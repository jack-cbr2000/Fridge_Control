#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <math.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <Update.h>
#include <time.h>

// Embedded HTML files (stored in flash memory)
#include "html_index.h"
#include "html_basic.h"
#include "html_manual.h"
#include "html_charts.h"
#include "html_settings.h"

// GitHub OTA Configuration
#define GITHUB_OWNER "jack-cbr2000"
#define GITHUB_REPO "Fridge_Control"
#define CURRENT_VERSION "1.0.6"
#define CHECK_INTERVAL_MINUTES 60

// NTP Configuration
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 0  // UTC
#define DAYLIGHT_OFFSET_SEC 3600  // +1 hour for DST if needed

// GitHub OTA variables
unsigned long lastUpdateCheck = 0;
bool otaUpdateInProgress = false;

// Structure to hold GitHub release info
struct GitHubRelease {
  String version;
  String downloadUrl;
  String releaseNotes;
  bool isNewer;
  time_t publishedAt;
};

// GitHub OTA forward declarations
GitHubRelease checkForUpdates();
bool downloadAndInstallFirmware(String firmwareUrl);
bool verifyFirmwareIntegrity();
bool rollbackFirmware();
void initiateManualUpdate();

// Temperature logging configuration
#define MAX_LOG_ENTRIES 500  // About 16 minutes at 2-second intervals

// Temperature log entry structure
struct TemperatureLogEntry {
  unsigned long timestamp;  // Unix timestamp (seconds)
  float leftTemp;
  float rightTemp;
  float setpointLeft;
  float setpointRight;
};

// Global temperature log buffer
TemperatureLogEntry tempLog[MAX_LOG_ENTRIES];
int logHead = 0;  // Next write position (circular)
int logCount = 0; // Number of valid entries

// Testing mode - set to false for production
#define TESTING_MODE false

// Pin definitions
#define NTC_LEFT_PIN 33
#define NTC_RIGHT_PIN 32
#define POTENTIOMETER_PIN 33  // Pin for temperature simulation
#define COMPRESSOR_PIN 4
#define SOLENOID_PIN 2
#define LED_PIN 5

// NTC thermistor constants (2.5k NTC)
#define THERMISTOR_NOMINAL 2500
#define TEMPERATURE_NOMINAL 25
#define BCOEFFICIENT 5000
#define SERIES_RESISTOR 2500

// WiFi Network structure
struct WiFiNetwork {
  char ssid[32] = "";
  char password[32] = "";
  bool enabled = true;
};

// Configuration structure - now supports 5 WiFi networks and auto-updates setting
struct Config {
  float leftSetpoint = 4.0;
  float rightSetpoint = 4.0;
  float hysteresis = 1.0;
  float minRunTime = 1.0;        // minutes
  float minStopTime = 5.0;       // minutes
  float minZoneSwitchTime = 5.0; // minutes
  float maxRunTime = 60.0;       // minutes
  float tempOffset = 0.0;
  bool leftEnabled = true;
  bool rightEnabled = true;
  bool autoUpdatesEnabled = true;  // Configurable auto-updates setting
  WiFiNetwork wifiNetworks[5];    // 5 WiFi network slots

  // NTC Calibration data (two-point calibration)
  bool ntcCalibrated = false;
  float calPoint1Temp = 25.0;     // Temperature for first calibration point (°C)
  float calPoint1ResistanceLeft = 2500;  // Resistance for left sensor at point 1
  float calPoint1ResistanceRight = 2500; // Resistance for right sensor at point 1
  float calPoint2Temp = 0.0;      // Temperature for second calibration point (°C)
  float calPoint2ResistanceLeft = 0;     // Resistance for left sensor at point 2
  float calPoint2ResistanceRight = 0;    // Resistance for right sensor at point 2
  float customBCoefficient = 0;   // Calculated beta coefficient (when calibrated)
  float customNominalTemp = 0;    // Custom nominal temperature (when calibrated)
  float customNominalResistance = 0;   // Custom nominal resistance at nominal temp

  // Backward compatibility - single network fields (now deprecated)
  char old_ssid[32] = "";
  char old_password[32] = "";
};

// System state
struct State {
  float leftTemp = 0.0;
  float rightTemp = 0.0;
  bool compressorOn = false;
  bool solenoidOn = false;    // true = right zone, false = left zone
  bool leftCooling = false;
  bool rightCooling = false;
  unsigned long lastCompressorStart = 0;
  unsigned long lastCompressorStop = 0;
  unsigned long lastZoneSwitch = 0;
  unsigned long lastTempRead = 0;
  int currentZone = 0;        // 0 = left, 1 = right
  bool systemEnabled = true;
  bool manualMode = false;    // Track if manual control is active
  String status = "Idle";
};

// DNS
const byte DNS_PORT = 53;
DNSServer dnsServer;

Config config;
State state;
WebServer server(80);  // Changed from 8080 to 80 for easier access

// --- Forward Declarations ---
void loadConfig();
void saveConfig();
void updateConfig(String jsonStr);
String getMainPage();
String getBasicPage();
String getManualPage();
String getChartsPage();
String getSettingsPage();
String getStatusJSON();
String getConfigJSON();
String getLogsJSON();
String getOtaStatusJSON();
void setupWebServer();
void readTemperatures();
void controlLogic();
void stopCompressor();
void switchZone(int zone);
void startCompressor(int zone);
String getMainPage();
void calculateNTCBeta();
float readNTCCalibrated(int pin, float avgResistanceLeft, float avgResistanceRight, bool isLeftSensor);

// GitHub OTA Implementation
GitHubRelease checkForUpdates();
bool downloadAndInstallFirmware(String firmwareUrl);
bool verifyFirmwareIntegrity();
bool rollbackFirmware();
void initiateManualUpdate();

// Simplified version comparison (assumes semantic versioning)
bool isVersionNewer(String remoteVersion, String currentVersion) {
  // Split versions by dots
  int remoteParts[3] = {0, 0, 0};
  int localParts[3] = {0, 0, 0};

  // Simple parsing - could be improved for complex version strings
  sscanf(remoteVersion.c_str(), "%d.%d.%d", &remoteParts[0], &remoteParts[1], &remoteParts[2]);
  sscanf(currentVersion.c_str(), "%d.%d.%d", &localParts[0], &localParts[1], &localParts[2]);

  if (remoteParts[0] > localParts[0]) return true;
  if (remoteParts[0] < localParts[0]) return false;
  if (remoteParts[1] > localParts[1]) return true;
  if (remoteParts[1] < localParts[1]) return false;
  return remoteParts[2] > localParts[2];
}

GitHubRelease checkForUpdates() {
  GitHubRelease result = {"", "", "", false, 0};

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ No WiFi connection for update check");
    return result;
  }

  HTTPClient http;
  String url = "https://api.github.com/repos/" + String(GITHUB_OWNER) + "/" + String(GITHUB_REPO) + "/releases/latest";

  Serial.println("🔍 Checking for firmware updates...");
  Serial.printf("📡 URL: %s\n", url.c_str());

  http.begin(url);
  http.addHeader("User-Agent", "ESP32-FridgeController/" + String(CURRENT_VERSION));
  http.setTimeout(15000);

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    // Use streaming to avoid loading entire response into memory
    WiFiClient* stream = http.getStreamPtr();
    
    // Create a filter to only parse the fields we need (saves memory)
    StaticJsonDocument<200> filter;
    filter["tag_name"] = true;
    filter["body"] = true;
    JsonObject filterAssets = filter["assets"].createNestedObject();
    filterAssets["name"] = true;
    filterAssets["browser_download_url"] = true;

    // Parse with filter - uses much less memory than parsing everything
    DynamicJsonDocument doc(3072);
    DeserializationError error = deserializeJson(doc, *stream, DeserializationOption::Filter(filter));

    if (error) {
      Serial.printf("❌ JSON parse error: %s\n", error.c_str());
      Serial.printf("   Memory usage: %d bytes\n", doc.memoryUsage());
      http.end();
      return result;
    }

    Serial.printf("✓ JSON parsed, memory used: %d bytes\n", doc.memoryUsage());

    // Extract firmware download URL from assets
    String firmwareUrl = "";
    JsonArray assets = doc["assets"];

    for (JsonVariant asset : assets) {
      String assetName = asset["name"].as<String>();
      if (assetName.startsWith("firmware-") && assetName.endsWith(".bin") &&
          assetName.indexOf("bootloader") == -1 && assetName.indexOf("partitions") == -1) {
        firmwareUrl = asset["browser_download_url"].as<String>();
        Serial.printf("📦 Found firmware: %s\n", assetName.c_str());
        break;
      }
    }

    result.version = doc["tag_name"].as<String>();
    result.downloadUrl = firmwareUrl;
    result.releaseNotes = doc["body"].as<String>();

    // Check if version is newer
    result.isNewer = isVersionNewer(result.version, CURRENT_VERSION);

    Serial.printf("📋 Latest version: %s\n", result.version.c_str());
    Serial.printf("📱 Current version: %s\n", CURRENT_VERSION);
    Serial.printf("✅ Update available: %s\n", result.isNewer ? "YES" : "NO");
    if (firmwareUrl.length() > 0) {
      Serial.printf("📥 Download URL: %s\n", firmwareUrl.c_str());
    } else {
      Serial.println("⚠️ No firmware asset found in release");
    }

  } else {
    Serial.printf("❌ HTTP error checking updates: %d\n", httpResponseCode);
    if (httpResponseCode == -1) {
      Serial.println("   Possible network timeout");
    }
  }

  http.end();
  return result;
}

bool downloadAndInstallFirmware(String firmwareUrl) {
  otaUpdateInProgress = true;
  Serial.println("⬇️ Starting firmware download...");

  if (!firmwareUrl.length()) {
    Serial.println("❌ No firmware URL provided");
    otaUpdateInProgress = false;
    return false;
  }

  HTTPClient http;
  Serial.printf("📡 Downloading from: %s\n", firmwareUrl.c_str());

  // Configure HTTP client to follow redirects (GitHub uses redirects for asset downloads)
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  http.setRedirectLimit(5);  // Allow up to 5 redirects
  
  http.begin(firmwareUrl);
  http.addHeader("User-Agent", "ESP32-FridgeController/" + String(CURRENT_VERSION));
  http.addHeader("Accept", "application/octet-stream");  // Request binary data
  http.setTimeout(60000);  // 60 second timeout for large downloads

  Serial.println("📡 Sending HTTP GET request...");
  int httpResponseCode = http.GET();
  Serial.printf("📡 HTTP Response: %d\n", httpResponseCode);

  if (httpResponseCode != 200) {
    Serial.printf("❌ Download failed: HTTP %d\n", httpResponseCode);
    if (httpResponseCode == 302 || httpResponseCode == 301) {
      Serial.println("   Redirect not followed - check HTTPClient configuration");
      String location = http.getLocation();
      if (location.length() > 0) {
        Serial.printf("   Redirect location: %s\n", location.c_str());
      }
    }
    http.end();
    otaUpdateInProgress = false;
    return false;
  }

  int contentLength = http.getSize();
  Serial.printf("📏 Firmware size: %d bytes\n", contentLength);

  if (contentLength <= 0 || contentLength > 2000000) {  // Max 2MB
    Serial.println("❌ Invalid content length");
    http.end();
    otaUpdateInProgress = false;
    return false;
  }

  // Start OTA update with progress callback
  if (!Update.begin(contentLength)) {
    Serial.printf("❌ Not enough space for OTA update: %s\n", Update.errorString());
    http.end();
    otaUpdateInProgress = false;
    return false;
  }

  Serial.println("🚀 Starting OTA update...");

  Update.onProgress([&](size_t done, size_t total) {
    static size_t lastReportedProgress = 0;
    size_t percentage = (done * 100) / total;
    if (percentage % 25 == 0 && percentage != lastReportedProgress) {
      Serial.printf("⚡ Progress: %d%%\n", percentage);
      lastReportedProgress = percentage;
    }
  });

  WiFiClient * stream = http.getStreamPtr();
  size_t written = Update.writeStream(*stream);

  if (written != (size_t)contentLength) {
    Serial.printf("❌ Write failed. Written: %d, Expected: %d\n", written, contentLength);
    Serial.printf("   Error: %s\n", Update.errorString());
    Update.abort();
    http.end();
    otaUpdateInProgress = false;
    return false;
  }

  if (!Update.end()) {
    Serial.printf("❌ OTA update failed: %s\n", Update.errorString());
    http.end();
    otaUpdateInProgress = false;
    return false;
  }

  if (!Update.isFinished()) {
    Serial.println("❌ Update not finished - something went wrong!");
    http.end();
    otaUpdateInProgress = false;
    return false;
  }

  http.end();

  // Update version file
  File file = LittleFS.open("/version.txt", "w");
  if (file) {
    GitHubRelease latest = checkForUpdates();  // Get the latest version
    file.printf("version=%s\n", latest.version.length() ? latest.version.c_str() : "unknown");
    file.printf("build_time=%lu\n", millis());
    file.printf("updated_from_github=true\n");
    file.close();
    Serial.println("✅ Version file updated");
  }

  Serial.println("✅ OTA update completed successfully!");
  otaUpdateInProgress = false;
  return true;
}

bool verifyFirmwareIntegrity() {
  // Basic integrity check - in production you might want to implement
  // SHA256 verification against known good hash
  return true;  // Placeholder
}

bool rollbackFirmware() {
  // In a production system, you might keep a backup of the previous firmware
  // For now, just disable auto-updates to prevent further issues
  config.autoUpdatesEnabled = false;
  saveConfig();
  Serial.println("⚠️ Firmware rollback initiated - auto-updates disabled");
  return true;
}

void initiateManualUpdate() {
  Serial.println("🔄 Manual update initiated");
  GitHubRelease update = checkForUpdates();

  if (update.isNewer) {
    Serial.printf("📦 Updating to version %s\n", update.version.c_str());
    if (downloadAndInstallFirmware(update.downloadUrl)) {
      Serial.println("✅ Manual update successful - restarting...");
      delay(2000);
      ESP.restart();
    } else {
      Serial.println("❌ Manual update failed");
    }
  } else {
    Serial.println("✅ Already on latest version");
  }
}

// OTA Status API endpoint
String getOtaStatusJSON() {
  DynamicJsonDocument doc(512);

  doc["currentVersion"] = CURRENT_VERSION;
  doc["autoUpdatesEnabled"] = config.autoUpdatesEnabled;
  doc["updateInProgress"] = otaUpdateInProgress;
  doc["lastChecked"] = lastUpdateCheck;
  doc["githubOwner"] = GITHUB_OWNER;
  doc["githubRepo"] = GITHUB_REPO;
  doc["checkIntervalMinutes"] = CHECK_INTERVAL_MINUTES;

  // Read version file if exists
  if (LittleFS.exists("/version.txt")) {
    File file = LittleFS.open("/version.txt", "r");
    if (file) {
      while (file.available()) {
        String line = file.readStringUntil('\n');
        if (line.startsWith("version=")) {
          doc["installedVersion"] = line.substring(8);
        } else if (line.startsWith("build_time=")) {
          doc["buildTime"] = line.substring(11);
        }
      }
      file.close();
    }
  }

  String output;
  serializeJson(doc, output);
  return output;
}

void setup() {
  Serial.begin(115200);

  Serial.println("Testing ADC pins (GPIO 32-39):");
  for(int pin = 32; pin <= 39; pin++) {
    int val = analogRead(pin);
    Serial.printf("GPIO%d: %d\n", pin, val);
  }
  delay(200); // Small delay to view results before continuing

  // Initialize pins
  pinMode(COMPRESSOR_PIN, OUTPUT);
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  digitalWrite(COMPRESSOR_PIN, HIGH); // Relay off (assuming active low)
  digitalWrite(SOLENOID_PIN, LOW);    // Left zone (default)
  digitalWrite(LED_PIN, LOW);
  
  // Initialize EEPROM and load config - increased size for calibration data
  EEPROM.begin(1024);
  loadConfig();
  
  // Initialize LittleFS (optional, for compatibility)
  if (!LittleFS.begin(false)) {
    Serial.println("LittleFS initialization failed - this is OK since we use embedded web files");
  } else {
    Serial.println("LittleFS initialized (optional)");
  }

  Serial.println("\n================================");
  Serial.println("🌐 DUAL ZONE FRIDGE CONTROLLER");
  Serial.println("================================");
  Serial.println("🎨 Using EMBEDDED WEB INTERFACE");
  Serial.println("   Web files are served from flash memory");
  Serial.println("   Updates via OTA will include new UI automatically!");
  Serial.println("================================");

  // Initialize WiFi - AP mode + STA mode
  Serial.println("\nInitializing WiFi...");
  
  // Start in AP+STA mode
  WiFi.mode(WIFI_AP_STA);
  
  // Configure Access Point
  String apSSID = "FridgeController_" + String((uint32_t)ESP.getEfuseMac(), HEX);
  String apPassword = "fridge123";
  WiFi.softAP(apSSID.c_str(), apPassword.c_str());
  
  Serial.printf("AP SSID: %s\n", apSSID.c_str());
  Serial.printf("AP Password: %s\n", apPassword.c_str());
  
  // Try to connect to configured WiFi networks (in priority order)
  bool connected = false;
  int connectedNetworkIndex = -1;

  Serial.println("\n--- Connecting to WiFi Networks ---");

  for (int i = 0; i < 5 && !connected; i++) {
    if (config.wifiNetworks[i].enabled && strlen(config.wifiNetworks[i].ssid) > 0) {
      Serial.printf("Attempting network %d/5: %s\n", i + 1, config.wifiNetworks[i].ssid);

      WiFi.disconnect();
      delay(100);

      WiFi.begin(config.wifiNetworks[i].ssid, config.wifiNetworks[i].password);

      // Wait up to 7 seconds for connection
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 14) {
        delay(500);
        Serial.print(".");
        attempts++;
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n✓ Connected to: %s\n", config.wifiNetworks[i].ssid);
        connected = true;
        connectedNetworkIndex = i;
        break;
      } else {
        Serial.printf("\n✗ Failed to connect to: %s\n", config.wifiNetworks[i].ssid);
      }
    }
  }

  if (!connected) {
    // Try compatibility mode - migrate old single network
    if (strlen(config.old_ssid) > 0) {
      Serial.printf("Migrating old network to multi-network: %s\n", config.old_ssid);

      // Migrate old network to first slot
      if (strlen(config.wifiNetworks[0].ssid) == 0) {
        strncpy(config.wifiNetworks[0].ssid, config.old_ssid, sizeof(config.wifiNetworks[0].ssid) - 1);
        strncpy(config.wifiNetworks[0].password, config.old_password, sizeof(config.wifiNetworks[0].password) - 1);
        config.wifiNetworks[0].enabled = true;

        // Try connecting to migrated network
        WiFi.disconnect();
        delay(100);
        WiFi.begin(config.wifiNetworks[0].ssid, config.wifiNetworks[0].password);

        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 14) {
          delay(500);
          Serial.print(".");
          attempts++;
        }

        if (WiFi.status() == WL_CONNECTED) {
          Serial.printf("\n✓ Connected to migrated network: %s\n", config.wifiNetworks[0].ssid);
          connected = true;
          connectedNetworkIndex = 0;
        } else {
          Serial.printf("\n✗ Failed to connect to migrated network\n");
        }
      }
    }
  }

  if (!connected) {
    Serial.println("⚠ No WiFi connection - AP mode only");
  }

  // Configure NTP if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n--- NTP Setup ---");
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    Serial.println("✓ NTP configured - timestamps will use real time");
  }

  delay(500); // Give WiFi time to stabilize
  
  // Display WiFi status
  Serial.println("\n--- WiFi Status ---");
  Serial.printf("AP Mode: %s\n", WiFi.getMode() & WIFI_AP ? "ENABLED" : "DISABLED");
  if (WiFi.getMode() & WIFI_AP) {
    Serial.printf("AP SSID: %s\n", WiFi.softAPSSID().c_str());
    Serial.printf("AP IP Address: %s\n", WiFi.softAPIP().toString().c_str());
    Serial.printf("👉 Access Point: http://%s\n", WiFi.softAPIP().toString().c_str());
  }
  
  Serial.printf("Station Mode: %s\n", WiFi.getMode() & WIFI_STA ? "ENABLED" : "DISABLED");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Connected to: %s\n", WiFi.SSID().c_str());
    Serial.printf("Station IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("👉 Local Network: http://%s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("Not connected to WiFi network");
  }

  // Start mDNS for both networks
  Serial.println("\n--- mDNS Setup ---");
  if (MDNS.begin("fridge")) {
    Serial.println("✓ mDNS responder started");
    Serial.println("👉 mDNS: http://fridge.local");
    MDNS.addService("http", "tcp", 80);  // Changed to port 80
  } else {
    Serial.println("❌ Error starting mDNS");
  }

  // Initialize OTA
  Serial.println("\n--- OTA Setup ---");
  ArduinoOTA.setHostname("FridgeController");
  ArduinoOTA.begin();
  Serial.println("✓ OTA ready for remote firmware updates");
  Serial.printf("👉 OTA: http://%s.local\n", ArduinoOTA.getHostname());

  // Setup web server routes
  Serial.println("\n--- Web Server Setup ---");
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());  // Catch-all DNS to AP IP
  Serial.println("✓ Captive portal DNS started");

  setupWebServer();
  
  server.begin();
  Serial.println("✓ Web server started on port 80");
  Serial.println("\n--- WiFi Config Portal ---");
  Serial.println("ℹ️ WiFi configuration available on port 8080");
  Serial.printf("👉 Config: http://%s:8080\n", WiFi.softAPIP().toString().c_str());

  // Final startup summary
  Serial.println("\n================================");
  Serial.println("✅ SYSTEM READY");
  Serial.println("================================");
  Serial.println("\n📱 TO ACCESS THE FRIDGE CONTROLLER:");
  Serial.println("1. Connect to WiFi AP: " + WiFi.softAPSSID());
  Serial.printf("2. Open browser to: http://%s\n", WiFi.softAPIP().toString().c_str());
  Serial.println("   OR try: http://fridge.local");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n🏠 Also available on local network:\n");
    Serial.printf("   http://%s\n", WiFi.localIP().toString().c_str());
  }
  Serial.println("\n================================\n");
  
  if (TESTING_MODE) {
    Serial.println("=== TESTING MODE ENABLED ===");
    Serial.println("Use potentiometer on pin 33 to simulate temperatures");
    Serial.println("Turn potentiometer to change temperature range");
    Serial.println("Set TESTING_MODE to false for production use");
    Serial.println("============================\n");
  }
}

void loop() {
  ArduinoOTA.handle();  // Handle OTA updates

  // Auto-reconnect to WiFi if disconnected - try all configured networks
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) {  // Check every 30 seconds
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, attempting to reconnect...");
      
      // Try each configured network in priority order
      for (int i = 0; i < 5; i++) {
        if (config.wifiNetworks[i].enabled && strlen(config.wifiNetworks[i].ssid) > 0) {
          Serial.printf("Trying network: %s\n", config.wifiNetworks[i].ssid);
          WiFi.begin(config.wifiNetworks[i].ssid, config.wifiNetworks[i].password);
          
          // Wait up to 5 seconds for connection
          int attempts = 0;
          while (WiFi.status() != WL_CONNECTED && attempts < 10) {
            delay(500);
            attempts++;
          }
          
          if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("Reconnected to: %s\n", config.wifiNetworks[i].ssid);
            break;
          }
        }
      }
    }
    lastWiFiCheck = millis();
  }

  server.handleClient();

  dnsServer.processNextRequest();

  // Read temperatures every 2 seconds
  if (millis() - state.lastTempRead > 2000) {
    readTemperatures();
    state.lastTempRead = millis();
  }

  // Main control logic (only if not in manual mode)
  if (!state.manualMode) {
    controlLogic();
  }

  // Check for firmware updates every CHECK_INTERVAL_MINUTES
  if (WiFi.status() == WL_CONNECTED && config.autoUpdatesEnabled &&
      millis() - lastUpdateCheck > (CHECK_INTERVAL_MINUTES * 60UL * 1000UL) &&
      !otaUpdateInProgress &&
      !state.compressorOn &&  // Only update when compressor is OFF (safe)
      millis() > (5 * 60 * 1000)) {  // Wait 5 minutes after boot before first check

    Serial.println("\n🔄 Automatic firmware update check...");
    GitHubRelease update = checkForUpdates();

    if (update.isNewer) {
      Serial.printf("📦 New firmware available: %s\n", update.version.c_str());
      Serial.println("⬇️ Starting automatic update...");

      if (downloadAndInstallFirmware(update.downloadUrl)) {
        Serial.println("✅ Firmware update successful!");
        Serial.println("🔄 ESP32 restarting in 3 seconds...");
        delay(3000);
        ESP.restart();
      } else {
        Serial.println("❌ Firmware update failed!");
        rollbackFirmware();
        // Try again in 24 hours
        lastUpdateCheck = millis() - (CHECK_INTERVAL_MINUTES * 60UL * 1000UL) + (24 * 60 * 60UL * 1000UL);
      }
    } else {
      Serial.println("✅ Firmware is up to date");
    }

    lastUpdateCheck = millis();
  }

  // Update LED status
  digitalWrite(LED_PIN, state.compressorOn);

  delay(100);
}

// Calculate Beta coefficient from two-point calibration
void calculateNTCBeta() {
  if (config.calPoint2ResistanceLeft > 0 && config.calPoint2ResistanceRight > 0) {
    // Calculate beta for left sensor: B = ln(R1/R2) / (1/T1 - 1/T2)
    float T1 = config.calPoint1Temp + 273.15;  // Convert to Kelvin
    float T2 = config.calPoint2Temp + 273.15;
    float R1 = config.calPoint1ResistanceLeft;
    float R2 = config.calPoint2ResistanceLeft;

    float beta = log(R1 / R2) / (1.0 / T1 - 1.0 / T2);
    config.customBCoefficient = beta;

    // Use the higher temperature point as nominal
    if (config.calPoint1Temp >= config.calPoint2Temp) {
      config.customNominalTemp = config.calPoint1Temp;
      config.customNominalResistance = config.calPoint1ResistanceLeft;
    } else {
      config.customNominalTemp = config.calPoint2Temp;
      config.customNominalResistance = config.calPoint2ResistanceLeft;
    }

    config.ntcCalibrated = true;
    Serial.printf("NTC calibration completed: Beta=%.1f, Nominal R=%.1f at %.1f°C\n",
      config.customBCoefficient, config.customNominalResistance, config.customNominalTemp);
  }
}

// Read NTC with optional custom calibration
float readNTC(int pin) {
  int reading = analogRead(pin);
  float resistance = SERIES_RESISTOR * (4095.0 / reading - 1.0);

  // Debug output
  Serial.printf("NTC Pin %d: ADC=%d, Resistance=%.1f ohm", pin, reading, resistance);

  float finalTemp;

  if (config.ntcCalibrated) {
    // Use custom calibrated values
    float steinhart = resistance / config.customNominalResistance;
    steinhart = log(steinhart);
    steinhart /= config.customBCoefficient;
    steinhart += 1.0 / (config.customNominalTemp + 273.15);
    steinhart = 1.0 / steinhart;
    steinhart -= 273.15;

    finalTemp = steinhart + config.tempOffset;
    Serial.printf(" Calibrated: %.2f°C (Beta=%.1f)", finalTemp, config.customBCoefficient);
  } else {
    // Use default values
    float steinhart = resistance / THERMISTOR_NOMINAL;
    steinhart = log(steinhart);
    steinhart /= BCOEFFICIENT;
    steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);
    steinhart = 1.0 / steinhart;
    steinhart -= 273.15;

    finalTemp = steinhart + config.tempOffset;
    Serial.printf(" Default: %.2f°C (raw:%.2f, offset:%.1f)", finalTemp, steinhart, config.tempOffset);
  }

  Serial.println();
  return finalTemp;
}

void readTemperatures() {
  if (TESTING_MODE) {
    // Testing mode - use potentiometer to simulate temperatures
    int potReading = analogRead(POTENTIOMETER_PIN);
    float baseTemp = map(potReading, 0, 4095, -10, 30); // Map to -10°C to 30°C range

    // Simulate different temperatures for each zone
    state.leftTemp = baseTemp + random(-20, 20) / 10.0; // Add some variation
    state.rightTemp = baseTemp + random(-20, 20) / 10.0;

    // Add some realistic simulation behavior
    if (state.compressorOn) {
      // Simulate cooling effect when compressor is running
      if (state.currentZone == 0) {
        state.leftTemp -= 0.1; // Slight cooling
      } else {
        state.rightTemp -= 0.1; // Slight cooling
      }
    }
  } else {
    // Production mode - read actual NTC sensors
    state.leftTemp = readNTC(NTC_LEFT_PIN);
    state.rightTemp = readNTC(NTC_RIGHT_PIN);
  }

  // Log temperature data for charts
  unsigned long currentTimestamp = time(nullptr); // Use Unix timestamp
  if (currentTimestamp < 1609459200) { // Jan 1, 2021 - fallback to millis-based timestamp
    currentTimestamp = millis() / 1000;
  }

  // Store in circular buffer
  tempLog[logHead] = {currentTimestamp, state.leftTemp, state.rightTemp, config.leftSetpoint, config.rightSetpoint};
  logHead = (logHead + 1) % MAX_LOG_ENTRIES;
  if (logCount < MAX_LOG_ENTRIES) {
    logCount++;
  }

  Serial.printf("Temps - Left: %.2f°C, Right: %.2f°C", state.leftTemp, state.rightTemp);
  if (TESTING_MODE) {
    Serial.print(" [TESTING MODE]");
  }
  if (state.manualMode) {
    Serial.print(" [MANUAL MODE]");
  }
  Serial.println();
}

void controlLogic() {
  unsigned long now = millis();
  
  if (!state.systemEnabled) {
    if (state.compressorOn) {
      stopCompressor();
    }
    state.status = "System Disabled";
    return;
  }
  
  // Check if zones need cooling
  bool leftNeedsCooling = config.leftEnabled && 
                         (state.leftTemp > config.leftSetpoint + config.hysteresis);
  bool rightNeedsCooling = config.rightEnabled && 
                          (state.rightTemp > config.rightSetpoint + config.hysteresis);
  
  // Check if zones are satisfied
  bool leftSatisfied = !config.leftEnabled || 
                      (state.leftTemp <= config.leftSetpoint - config.hysteresis);
  bool rightSatisfied = !config.rightEnabled || 
                       (state.rightTemp <= config.rightSetpoint - config.hysteresis);
  
  // Determine which zone to cool
  int targetZone = -1;
  if (leftNeedsCooling && rightNeedsCooling) {
    // Both need cooling - choose the one that's warmer
    targetZone = (state.leftTemp > state.rightTemp) ? 0 : 1;
  } else if (leftNeedsCooling) {
    targetZone = 0;
  } else if (rightNeedsCooling) {
    targetZone = 1;
  }
  
  // Compressor control logic
  if (state.compressorOn) {
    // Compressor is running
    bool shouldStop = false;
    
    // Check max run time
    if (now - state.lastCompressorStart > config.maxRunTime * 60000) {
      shouldStop = true;
      state.status = "Max run time reached";
    }
    
    // Check if current zone is satisfied
    if (state.currentZone == 0 && leftSatisfied) {
      shouldStop = true;
      state.status = "Left zone satisfied";
    } else if (state.currentZone == 1 && rightSatisfied) {
      shouldStop = true;
      state.status = "Right zone satisfied";
    }
    
    // Check minimum run time before allowing stop
    if (shouldStop && (now - state.lastCompressorStart >= config.minRunTime * 60000)) {
      stopCompressor();
    }
    
    // Check if we need to switch zones
    if (!shouldStop && targetZone != -1 && targetZone != state.currentZone) {
      if (now - state.lastZoneSwitch >= config.minZoneSwitchTime * 60000) {
        switchZone(targetZone);
      }
    }
  } else {
    // Compressor is off
    if (targetZone != -1) {
      // Check minimum stop time
      if (now - state.lastCompressorStop >= config.minStopTime * 60000) {
        startCompressor(targetZone);
      } else {
        state.status = "Waiting for min stop time";
      }
    } else {
      if (leftSatisfied && rightSatisfied) {
        state.status = "Both zones satisfied";
      } else if (leftSatisfied) {
        state.status = "Left zone satisfied";
      } else if (rightSatisfied) {
        state.status = "Right zone satisfied";
      } else {
        state.status = "Idle";
      }
    }
  }
}

void startCompressor(int zone) {
  state.compressorOn = true;
  state.lastCompressorStart = millis();

  switchZone(zone);  // Sets state.currentZone

  // Set cooling flags directly
  state.leftCooling = (zone == 0);
  state.rightCooling = (zone == 1);

  digitalWrite(COMPRESSOR_PIN, HIGH); // Turn on compressor (assuming active high)
  state.status = String("Cooling ") + (zone == 0 ? "Left" : "Right") + " zone";
  Serial.println("Compressor started - " + state.status);
}

void stopCompressor() {
  state.compressorOn = false;
  state.lastCompressorStop = millis();

  // Reset cooling flags
  state.leftCooling = false;
  state.rightCooling = false;

  digitalWrite(COMPRESSOR_PIN, LOW); // Turn off compressor
  state.status = "Compressor stopped";
  Serial.println("Compressor stopped");
}

void switchZone(int zone) {
  if (zone != state.currentZone) {
    state.lastZoneSwitch = millis();
  }

  state.currentZone = zone;
  state.solenoidOn = (zone == 1);
  digitalWrite(SOLENOID_PIN, zone == 1 ? HIGH : LOW);
  Serial.printf("Switched to %s zone\n", zone == 0 ? "Left" : "Right");
}

void setupWebServer() {
  // Captive portal detection handlers - redirect to main page
  server.on("/generate_204", HTTP_GET, []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.on("/redirect", HTTP_GET, []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.on("/ncsi.txt", HTTP_GET, []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.on("/connecttest.txt", HTTP_GET, []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.on("/success.txt", HTTP_GET, []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.on("/hotspot-detect.html", HTTP_GET, []() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  // Serve pages
  server.on("/", []() {
    server.send(200, "text/html", getMainPage());
  });

  server.on("/basic", []() {
    server.send(200, "text/html", getBasicPage());
  });

  server.on("/manual", []() {
    server.send(200, "text/html", getManualPage());
  });

  server.on("/charts", []() {
    server.send(200, "text/html", getChartsPage());
  });

  server.on("/settings", []() {
    server.send(200, "text/html", getSettingsPage());
  });

  // API endpoints
  server.on("/api/status", HTTP_GET, []() {
    server.send(200, "application/json", getStatusJSON());
  });

  server.on("/api/config", HTTP_GET, []() {
    server.send(200, "application/json", getConfigJSON());
  });

  server.on("/api/logs", HTTP_GET, []() {
    server.send(200, "application/json", getLogsJSON());
  });

  server.on("/api/ota/status", HTTP_GET, []() {
    server.send(200, "application/json", getOtaStatusJSON());
  });

  // Check for updates (does NOT install - just returns version info)
  server.on("/api/ota/check", HTTP_GET, []() {
    Serial.println("🔍 Checking for updates via web interface...");
    GitHubRelease update = checkForUpdates();
    lastUpdateCheck = millis();

    DynamicJsonDocument doc(1024);
    doc["success"] = true;
    doc["currentVersion"] = CURRENT_VERSION;
    doc["latestVersion"] = update.version;
    doc["updateAvailable"] = update.isNewer;
    doc["downloadUrl"] = update.downloadUrl;
    doc["releaseNotes"] = update.releaseNotes;

    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
  });

  // Install update (assumes check already done, proceeds with download/install)
  server.on("/api/ota/update", HTTP_POST, []() {
    Serial.println("🔄 Manual OTA install triggered via web interface");
    
    // Get the download URL from the request body if provided, otherwise check again
    String downloadUrl = "";
    if (server.hasArg("plain")) {
      DynamicJsonDocument doc(512);
      deserializeJson(doc, server.arg("plain"));
      if (doc.containsKey("downloadUrl")) {
        downloadUrl = doc["downloadUrl"].as<String>();
      }
    }

    // If no URL provided, check for updates to get the URL
    if (downloadUrl.length() == 0) {
      GitHubRelease update = checkForUpdates();
      if (!update.isNewer) {
        server.send(200, "application/json", "{\"success\":false,\"message\":\"No updates available\",\"currentVersion\":\"" + String(CURRENT_VERSION) + "\"}");
        return;
      }
      downloadUrl = update.downloadUrl;
    }

    if (downloadUrl.length() == 0) {
      server.send(400, "application/json", "{\"success\":false,\"message\":\"No firmware download URL available\"}");
      return;
    }

    Serial.printf("📦 Installing update from: %s\n", downloadUrl.c_str());
    
    // Send response before starting update (update will restart ESP)
    server.send(200, "application/json", "{\"success\":true,\"message\":\"Update installation started. Device will restart when complete.\"}");
    
    // Small delay to ensure response is sent
    delay(100);
    
    if (downloadAndInstallFirmware(downloadUrl)) {
      Serial.println("✅ Update successful - restarting...");
      delay(1000);
      ESP.restart();
    } else {
      Serial.println("❌ Update failed");
      // Can't send response here since we already sent one
    }
  });
  
  server.on("/api/config", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      updateConfig(server.arg("plain"));
      server.send(200, "application/json", "{\"success\":true}");
    } else {
      server.send(400, "application/json", "{\"error\":\"No data\"}");
    }
  });
  
  server.on("/api/system", HTTP_POST, []() {
    if (server.hasArg("action")) {
      String action = server.arg("action");
      if (action == "enable") {
        state.systemEnabled = true;
        state.manualMode = false; // Exit manual mode when enabling system
      } else if (action == "disable") {
        state.systemEnabled = false;
      }
      server.send(200, "application/json", "{\"success\":true}");
    }
  });

  // Auto-updates toggle endpoint
  server.on("/api/ota/auto-updates", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      DynamicJsonDocument doc(128);
      deserializeJson(doc, server.arg("plain"));

      if (doc.containsKey("enabled")) {
        config.autoUpdatesEnabled = doc["enabled"];
        saveConfig();
        server.send(200, "application/json", "{\"success\":true,\"autoUpdatesEnabled\":" + String(config.autoUpdatesEnabled ? "true" : "false") + "}");
        Serial.printf("Auto-updates %s\n", config.autoUpdatesEnabled ? "enabled" : "disabled");
      } else {
        server.send(400, "application/json", "{\"error\":\"Missing enabled field\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"No data\"}");
    }
  });

// Manual control endpoints
server.on("/api/manual/compressor", HTTP_POST, []() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    if (action == "on") {
      // Manual compressor on
      state.manualMode = true;
      digitalWrite(COMPRESSOR_PIN, HIGH); // Turn on compressor (assuming active high)
      state.compressorOn = true;
      state.status = "Manual Compressor ON";
      Serial.println("Manual: Compressor turned ON");
    } else if (action == "off") {
      // Manual compressor off
      state.manualMode = true;
      digitalWrite(COMPRESSOR_PIN, LOW); // Turn off compressor
      state.compressorOn = false;
      state.leftCooling = false;
      state.rightCooling = false;
      state.status = "Manual Compressor OFF";
      Serial.println("Manual: Compressor turned OFF");
    } else if (action == "auto") {
      // Return to automatic control
      state.manualMode = false;
      state.status = "Returned to automatic control";
      Serial.println("Manual: Returned to automatic control");
    }
    server.send(200, "application/json", "{\"success\":true}");
  } else {
    server.send(400, "application/json", "{\"error\":\"No action specified\"}");
  }
});
  
  server.on("/api/manual/solenoid", HTTP_POST, []() {
    if (server.hasArg("action")) {
      String action = server.arg("action");
      if (action == "left") {
        state.manualMode = true;
        digitalWrite(SOLENOID_PIN, LOW);
        state.solenoidOn = false;
        state.currentZone = 0;
        state.status = "Manual Solenoid: LEFT zone";
        Serial.println("Manual: Solenoid switched to LEFT zone");
      } else if (action == "right") {
        state.manualMode = true;
        digitalWrite(SOLENOID_PIN, HIGH);
        state.solenoidOn = true;
        state.currentZone = 1;
        state.status = "Manual Solenoid: RIGHT zone";
        Serial.println("Manual: Solenoid switched to RIGHT zone");
      } else if (action == "auto") {
        // Return to automatic control
        state.manualMode = false;
        state.status = "Returned to automatic control";
        Serial.println("Manual: Returned to automatic control");
      }
      server.send(200, "application/json", "{\"success\":true}");
    } else {
      server.send(400, "application/json", "{\"error\":\"No action specified\"}");
    }
  });

  // WiFi network scan endpoint
  server.on("/api/wifi/scan", HTTP_GET, []() {
    Serial.println("WiFi scan requested");
    int n = WiFi.scanNetworks();
    
    DynamicJsonDocument doc(2048);
    JsonArray networks = doc.to<JsonArray>();
    
    for (int i = 0; i < n; i++) {
      JsonObject network = networks.createNestedObject();
      network["ssid"] = WiFi.SSID(i);
      network["rssi"] = WiFi.RSSI(i);
      network["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured";
      
      // Calculate signal strength percentage
      int rssi = WiFi.RSSI(i);
      int quality;
      if (rssi <= -100) {
        quality = 0;
      } else if (rssi >= -50) {
        quality = 100;
      } else {
        quality = 2 * (rssi + 100);
      }
      network["quality"] = quality;
    }
    
    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
    Serial.printf("Found %d networks\n", n);
  });

  // Multi-network WiFi configuration endpoint
  server.on("/api/wifi/config", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      DynamicJsonDocument doc(512);
      deserializeJson(doc, server.arg("plain"));

      if (doc.containsKey("networks")) {
        // Handle array of networks
        JsonArray networks = doc["networks"].as<JsonArray>();
        int networkCount = 0;

        // Clear existing network config
        memset((void*)&config.wifiNetworks, 0, sizeof(config.wifiNetworks));

        // Add each network
        for (JsonObject network : networks) {
          if (networkCount >= 5) break; // Max 5 networks

          String ssid = network["ssid"].as<String>();
          String password = network["password"].as<String>();

          if (ssid.length() > 0) {
            strncpy(config.wifiNetworks[networkCount].ssid, ssid.c_str(), sizeof(config.wifiNetworks[networkCount].ssid) - 1);
            config.wifiNetworks[networkCount].ssid[sizeof(config.wifiNetworks[networkCount].ssid) - 1] = '\0';

            if (password.length() > 0) {
              strncpy(config.wifiNetworks[networkCount].password, password.c_str(), sizeof(config.wifiNetworks[networkCount].password) - 1);
              config.wifiNetworks[networkCount].password[sizeof(config.wifiNetworks[networkCount].password) - 1] = '\0';
            }

            config.wifiNetworks[networkCount].enabled = true;
            networkCount++;
          }
        }

        saveConfig();
        Serial.printf("Multi-network WiFi config saved: %d networks\n", networkCount);

        // Trigger reconnection attempt
        WiFi.disconnect();

        server.send(200, "application/json", "{\"success\":true,\"message\":\"Multi-network WiFi configuration saved. Controller will attempt to connect.\"}");
      }
      else if (doc.containsKey("ssid")) {
        // Fallback for single network (backward compatibility)
        String newSSID = doc["ssid"].as<String>();
        String newPassword = doc.containsKey("password") ? doc["password"].as<String>() : "";

        // Clear existing networks and add single network
        memset((void*)&config.wifiNetworks, 0, sizeof(config.wifiNetworks));

        if (newSSID.length() > 0) {
          strncpy(config.wifiNetworks[0].ssid, newSSID.c_str(), sizeof(config.wifiNetworks[0].ssid) - 1);
          config.wifiNetworks[0].ssid[sizeof(config.wifiNetworks[0].ssid) - 1] = '\0';

          if (newPassword.length() > 0) {
            strncpy(config.wifiNetworks[0].password, newPassword.c_str(), sizeof(config.wifiNetworks[0].password) - 1);
            config.wifiNetworks[0].password[sizeof(config.wifiNetworks[0].password) - 1] = '\0';
          }

          config.wifiNetworks[0].enabled = true;
        }

        saveConfig();
        Serial.printf("Single-network WiFi config saved: %s\n", config.wifiNetworks[0].ssid);

        // Attempt to connect
        WiFi.disconnect();
        delay(100);
        WiFi.begin(config.wifiNetworks[0].ssid, config.wifiNetworks[0].password);

        server.send(200, "application/json", "{\"success\":true,\"message\":\"WiFi credentials saved. Connecting...\"}");
      } else {
        server.send(400, "application/json", "{\"error\":\"Missing network configuration\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"No data\"}");
    }
  });

  // Get current WiFi networks configuration
  server.on("/api/wifi/networks", HTTP_GET, []() {
    DynamicJsonDocument doc(1024);
    JsonArray networksArray = doc.createNestedArray("networks");

    for (int i = 0; i < 5; i++) {
      JsonObject networkObj = networksArray.createNestedObject();
      networkObj["id"] = i;
      networkObj["ssid"] = config.wifiNetworks[i].ssid;
      networkObj["password"] = strlen(config.wifiNetworks[i].password) > 0 ? "********" : "";
      networkObj["enabled"] = config.wifiNetworks[i].enabled;
      networkObj["priority"] = i + 1; // 1-based priority
    }

    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
  });

  // NTC Calibration endpoints
  server.on("/api/calibration/point1", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      DynamicJsonDocument doc(128);
      deserializeJson(doc, server.arg("plain"));

      if (doc.containsKey("actualTemp")) {
        float actualTemp = doc["actualTemp"];

        // Record resistances at current readings
        int leftReading = analogRead(NTC_LEFT_PIN);
        int rightReading = analogRead(NTC_RIGHT_PIN);

        config.calPoint1ResistanceLeft = SERIES_RESISTOR * (4095.0 / leftReading - 1.0);
        config.calPoint1ResistanceRight = SERIES_RESISTOR * (4095.0 / rightReading - 1.0);
        config.calPoint1Temp = actualTemp;

        saveConfig();
        Serial.printf("Calibration Point 1 set: %.1f°C, R_left=%.1f, R_right=%.1f\n",
          actualTemp, config.calPoint1ResistanceLeft, config.calPoint1ResistanceRight);

        server.send(200, "application/json", "{\"success\":true,\"point\":1,\"resistanceLeft\":" +
          String(config.calPoint1ResistanceLeft) + ",\"resistanceRight\":" +
          String(config.calPoint1ResistanceRight) + "}");
      } else {
        server.send(400, "application/json", "{\"error\":\"Missing actualTemp\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"No data\"}");
    }
  });

  server.on("/api/calibration/point2", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      DynamicJsonDocument doc(128);
      deserializeJson(doc, server.arg("plain"));

      if (doc.containsKey("actualTemp")) {
        float actualTemp = doc["actualTemp"];

        // Record resistances at current readings
        int leftReading = analogRead(NTC_LEFT_PIN);
        int rightReading = analogRead(NTC_RIGHT_PIN);

        config.calPoint2ResistanceLeft = SERIES_RESISTOR * (4095.0 / leftReading - 1.0);
        config.calPoint2ResistanceRight = SERIES_RESISTOR * (4095.0 / rightReading - 1.0);
        config.calPoint2Temp = actualTemp;

        // Calculate beta coefficient
        calculateNTCBeta();
        saveConfig();

        Serial.printf("Calibration Point 2 set: %.1f°C, R_left=%.1f, R_right=%.1f\n",
          actualTemp, config.calPoint2ResistanceLeft, config.calPoint2ResistanceRight);

        String response = "{\"success\":true,\"point\":2,\"calibrated\":" +
          String(config.ntcCalibrated ? "true" : "false") + ",\"beta\":" +
          String(config.customBCoefficient, 1) + ",\"nominalTemp\":" +
          String(config.customNominalTemp, 1) + ",\"nominalResistance\":" +
          String(config.customNominalResistance, 1) + "}";

        server.send(200, "application/json", response);
      } else {
        server.send(400, "application/json", "{\"error\":\"Missing actualTemp\"}");
      }
    } else {
      server.send(400, "application/json", "{\"error\":\"No data\"}");
    }
  });

  server.on("/api/calibration/status", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    doc["ntcCalibrated"] = config.ntcCalibrated;

    if (config.calPoint1ResistanceLeft > 0) {
      doc["point1"]["temp"] = config.calPoint1Temp;
      doc["point1"]["resistanceLeft"] = config.calPoint1ResistanceLeft;
      doc["point1"]["resistanceRight"] = config.calPoint1ResistanceRight;
    } else {
      doc["point1"] = nullptr;
    }

    if (config.calPoint2ResistanceLeft > 0) {
      doc["point2"]["temp"] = config.calPoint2Temp;
      doc["point2"]["resistanceLeft"] = config.calPoint2ResistanceLeft;
      doc["point2"]["resistanceRight"] = config.calPoint2ResistanceRight;
    } else {
      doc["point2"] = nullptr;
    }

    if (config.ntcCalibrated) {
      doc["customBeta"] = config.customBCoefficient;
      doc["nominalTemp"] = config.customNominalTemp;
      doc["nominalResistance"] = config.customNominalResistance;
    }

    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
  });

  server.on("/api/calibration/reset", HTTP_POST, []() {
    config.ntcCalibrated = false;
    config.calPoint1ResistanceLeft = 2500;
    config.calPoint1ResistanceRight = 2500;
    config.calPoint2ResistanceLeft = 0;
    config.calPoint2ResistanceRight = 0;
    config.customBCoefficient = 0;
    config.customNominalTemp = 0;
    config.customNominalResistance = 0;

    saveConfig();
    Serial.println("NTC calibration reset to defaults");

    server.send(200, "application/json", "{\"success\":true,\"message\":\"Calibration reset\"}");
  });
}

String getMainPage() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Failed to open /index.html from LittleFS");
    return "<html><body><h1>File Not Found</h1><p>Could not load index.html from LittleFS.</p><p>Please upload filesystem image!</p></body></html>";
  }

  String content = "";
  while (file.available()) {
    content += (char)file.read();
  }
  file.close();

  return content;
}

String getBasicPage() {
  Serial.println("Serving embedded basic.html");
  return FPSTR(HTML_BASIC);
}

String getManualPage() {
  Serial.println("Serving embedded manual.html");
  return FPSTR(HTML_MANUAL);
}

String getChartsPage() {
  Serial.println("Serving embedded charts.html");
  return FPSTR(HTML_CHARTS);
}

String getSettingsPage() {
  Serial.println("Serving embedded settings.html");
  return FPSTR(HTML_SETTINGS);
}

String getStatusJSON() {
  DynamicJsonDocument doc(1024);
  doc["leftTemp"] = state.leftTemp;
  doc["rightTemp"] = state.rightTemp;
  doc["compressorOn"] = state.compressorOn;
  doc["leftCooling"] = state.leftCooling;
  doc["rightCooling"] = state.rightCooling;
  doc["currentZone"] = state.currentZone;
  doc["currentZoneName"] = state.currentZone == 0 ? "Left" : "Right";
  doc["systemEnabled"] = state.systemEnabled;
  doc["manualMode"] = state.manualMode;
  doc["status"] = state.status;
  doc["uptime"] = millis() / 1000;
  doc["testingMode"] = TESTING_MODE;

  // Network status
  doc["wifiConnected"] = (WiFi.status() == WL_CONNECTED);
  doc["wifiSSID"] = WiFi.SSID();
  doc["wifiIP"] = WiFi.localIP().toString();
  doc["apIP"] = WiFi.softAPIP().toString();

  String output;
  serializeJson(doc, output);
  return output;
}

String getConfigJSON() {
  DynamicJsonDocument doc(1024);
  doc["leftSetpoint"] = config.leftSetpoint;
  doc["rightSetpoint"] = config.rightSetpoint;
  doc["hysteresis"] = config.hysteresis;
  doc["minRunTime"] = config.minRunTime;
  doc["minStopTime"] = config.minStopTime;
  doc["minZoneSwitchTime"] = config.minZoneSwitchTime;
  doc["maxRunTime"] = config.maxRunTime;
  doc["tempOffset"] = config.tempOffset;
  doc["leftEnabled"] = config.leftEnabled;
  doc["rightEnabled"] = config.rightEnabled;

  String output;
  serializeJson(doc, output);
  return output;
}



void updateConfig(String jsonStr) {
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, jsonStr);

  if (doc.containsKey("leftSetpoint")) config.leftSetpoint = doc["leftSetpoint"];
  if (doc.containsKey("rightSetpoint")) config.rightSetpoint = doc["rightSetpoint"];
  if (doc.containsKey("hysteresis")) config.hysteresis = doc["hysteresis"];
  if (doc.containsKey("minRunTime")) config.minRunTime = doc["minRunTime"];
  if (doc.containsKey("minStopTime")) config.minStopTime = doc["minStopTime"];
  if (doc.containsKey("minZoneSwitchTime")) config.minZoneSwitchTime = doc["minZoneSwitchTime"];
  if (doc.containsKey("maxRunTime")) config.maxRunTime = doc["maxRunTime"];
  if (doc.containsKey("tempOffset")) config.tempOffset = doc["tempOffset"];
  if (doc.containsKey("leftEnabled")) config.leftEnabled = doc["leftEnabled"];
  if (doc.containsKey("rightEnabled")) config.rightEnabled = doc["rightEnabled"];

  // WiFi config no longer handled here - managed by ESPWifiConfig library

  saveConfig();
}

void saveConfig() {
  EEPROM.put(0, config);
  EEPROM.commit();
}



bool factoryDefaultsLoaded = false;

void loadConfig() {
  EEPROM.get(0, config);

  if (isnan(config.leftSetpoint) || config.leftSetpoint < -20 || config.leftSetpoint > 10) {
    config.leftSetpoint = 4.0;
    factoryDefaultsLoaded = true;
  }

  if (isnan(config.rightSetpoint) || config.rightSetpoint < -20 || config.rightSetpoint > 10) {
    config.rightSetpoint = 4.0;
    factoryDefaultsLoaded = true;
  }

  if (isnan(config.hysteresis) || config.hysteresis < 1.0 || config.hysteresis > 8.0) {
    config.hysteresis = 1.5;
    factoryDefaultsLoaded = true;
  }

  if (isnan(config.minRunTime) || config.minRunTime < 0.5 || config.minRunTime > 10.0) {
    config.minRunTime = 1.0;
    factoryDefaultsLoaded = true;
  }

  if (isnan(config.minStopTime) || config.minStopTime < 1.0 || config.minStopTime > 15.0) {
    config.minStopTime = 5.0;
    factoryDefaultsLoaded = true;
  }

  if (isnan(config.minZoneSwitchTime) || config.minZoneSwitchTime < 1.0 || config.minZoneSwitchTime > 10.0) {
    config.minZoneSwitchTime = 5.0;
    factoryDefaultsLoaded = true;
  }

  if (isnan(config.maxRunTime) || config.maxRunTime < 10.0 || config.maxRunTime > 45.0) {
    config.maxRunTime = 30.0;
    factoryDefaultsLoaded = true;
  }

  if (isnan(config.tempOffset) || config.tempOffset < -5.0 || config.tempOffset > 5.0) {
    config.tempOffset = 0.0;
    factoryDefaultsLoaded = true;
  }

  if (config.leftEnabled != true && config.leftEnabled != false) {
    config.leftEnabled = true;
    factoryDefaultsLoaded = true;
  }

  if (config.rightEnabled != true && config.rightEnabled != false) {
    config.rightEnabled = true;
    factoryDefaultsLoaded = true;
  }

  // Initialize calibration defaults if not properly set
  if (config.calPoint1ResistanceLeft <= 0 || isnan(config.calPoint1ResistanceLeft)) {
    config.calPoint1ResistanceLeft = 2500;
    config.calPoint1ResistanceRight = 2500;
    config.calPoint1Temp = 25.0;
  }

  if (config.calPoint2ResistanceLeft >= 0 && isnan(config.calPoint2ResistanceLeft)) {
    config.calPoint2ResistanceLeft = 0;
    config.calPoint2ResistanceRight = 0;
    config.calPoint2Temp = 0.0;
  }

  if (isnan(config.customBCoefficient) || config.customBCoefficient <= 0) {
    config.customBCoefficient = 0;
    config.customNominalTemp = 0;
    config.customNominalResistance = 0;
    config.ntcCalibrated = false;
  }

  if (factoryDefaultsLoaded) {
    saveConfig();  // commit restored defaults to EEPROM
    Serial.println("⚠️ Factory defaults loaded due to invalid or missing config.");
  }
}

String getLogsJSON() {
  DynamicJsonDocument doc(4096); // Larger buffer for log data

  // Create root array
  JsonArray logs = doc.to<JsonArray>();

  // Return all available log entries (may be less than MAX_LOG_ENTRIES)
  for (int i = 0; i < logCount; i++) {
    JsonObject entry = logs.createNestedObject();
    entry["timestamp"] = tempLog[i].timestamp;
    entry["leftTemp"] = tempLog[i].leftTemp;
    entry["rightTemp"] = tempLog[i].rightTemp;
    entry["setpointLeft"] = tempLog[i].setpointLeft;
    entry["setpointRight"] = tempLog[i].setpointRight;
  }

  String output;
  serializeJson(doc, output);
  return output;
}
