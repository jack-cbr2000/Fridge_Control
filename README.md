# Fridge_Control - ESP32 Dual Zone Fridge Controller with OTA Updates

This ESP32-based project implements a dual-zone refrigerator controller with advanced web interface, temperature logging with interactive charts, and **automatic over-the-air firmware updates** through GitHub.

## Features

- **Dual Zone Control**: Independent temperature control for two separate cooling zones
- **Web Interface**: Responsive web UI accessible via WiFi for monitoring and configuration
- **Temperature Logging**: Real-time charts showing temperature history for both zones
- **OTA Firmware Updates**: Automatic updates through GitHub Actions (safe updates only when compressor is OFF)
- **Automatic Control**: Intelligent cooling logic with configurable parameters (hysteresis, timing, setpoints)
- **Manual Override**: Full manual control of compressor and zone switching for maintenance/diagnosis
- **Temperature Sensing**: NTC thermistor sensors for accurate temperature monitoring
- **Relay Control**: 2-relay system for compressor and solenoid valve operation
- **System Status**: Real-time status monitoring and error handling
- **Configuration Storage**: Persistent settings stored in EEPROM
- **WiFi Access Point**: Built-in hotspot for initial setup and standalone operation
- **OTA Updates**: Over-the-air firmware updates via ArduinoOTA
- **Testing Mode**: Hardware-validated testing mode using potentiometer for temperature simulation
- **Multi-Network WiFi**: Support for up to 5 WiFi networks with automatic failover
- **GitHub Integration**: Automated CI/CD pipeline for firmware builds and releases

## Hardware Requirements

### Core Components
- **ESP32 Development Board** (any ESP32 board with WiFi)
- **2x Relay Modules** (5V coil, SPDT contacts rated for respective loads)
- **NTC Thermistors** (10K B=3435 or B=5000 type)
- **Solenoid Valve** (12VDC for refrigerant flow control)
- **Status LED** (optional, 3mm LED with current-limiting resistor)

### Power Supplies
- **12V DC Supply**: Main power for entire refrigerator system
- **5V Supply**: For ESP32 and relay modules (USB or regulated 5V adapter from 12V supply)
- **12V Relay Supply**: Direct from main 12V system power

### Relay Specifications
- **Compressor Relay**: DC-rated SPDT relay
  - Coil: 5VDC with ESP32-compatible current draw (<40mA)
  - Contacts: Rated for compressor motor current + 20% safety margin
  - Typical: 5A/12VDC contacts (or higher based on compressor specifications)
- **Solenoid Relay**: DC-rated SPDT relay
  - Coil: 5VDC with ESP32-compatible current draw (<40mA)
  - Contacts: Rated for solenoid current (typically 0.5-1A @ 12VDC)
  - Opto-isolated with no additional flyback diode required

## Pin Configuration

### ESP32 Pin Map
```
Pin 32: Left zone NTC thermistor (ADC input)
Pin 33: Right zone NTC thermistor (ADC input)

Pin 2:  Compressor relay control (digital output)
Pin 4:  Solenoid valve relay control (digital output)
Pin 5:  Status LED (optional, digital output)
GND:    Common ground for all circuits
```

### NTC Thermistor Circuit
Each NTC connects to ADC pin through voltage divider:
```
   +3.3V ──── NTC 10K ──── ADC Pin ──── 10K Resistor ──── GND
                          │
                       │││ (NTC thermistor)
```

## Relay Wiring Diagram (12V DC System)

### Compressor Relay
```
ESP32 GPIO 2 ──► Relay Input
5V Power ─┬──► Relay VCC
GND ──────┼──► Relay GND

Relay Contacts (COM/NO/NC):
12V Supply ─┬─► Compressor Motor (+)
            │
      Relay │
            └─► Compressor Motor (-)
```

### Solenoid Relay (Opto-Isolated)
```
ESP32 GPIO 4 ──► Relay Input
5V Power ─┬──► Relay VCC
GND ──────┼──► Relay GND
           *Opto-isolation: No external diode needed*

Relay Contacts (COM/NO/NC):
12V Supply ─┬─► Solenoid Valve (+)
            │
      Relay │
            └─► Solenoid Valve (-)
```

## Software Setup

### PlatformIO (Recommended)

1. **Install PlatformIO**:
   ```bash
   pip install platformio
   ```

2. **Clone the repository**:
   ```bash
   git clone https://github.com/jack-cbr2000/Fridge_Control.git
   cd Fridge_Control
   ```

3. **Build the project**:
   ```bash
   pio run
   ```

4. **Upload firmware**:
   ```bash
   pio run -t upload
   ```

5. **Upload web files to LittleFS**:
   ```bash
   pio run -t uploadfs
   ```

### Arduino IDE

1. **Install Arduino IDE 2.x**: https://www.arduino.cc/en/software

2. **Add ESP32 Board Support**:
   - File → Preferences → Additional Boards Manager URLs: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Tools → Board → Boards Manager → Install "ESP32 by Espressif Systems"

3. **Install Required Libraries**:
   - ArduinoJson, ESPmDNS, DNSServer, ESP32Servo (if needed)

4. **Load Files**:
   - Open `src/main.cpp`
   - Select ESP32 board and correct COM port
   - Upload

## OTA Firmware Updates (New!)

### Automatic Updates
- **Safe Updates**: Only updates when compressor is OFF (prevents fridge damage)
- **Hourly Checks**: Automatically checks for updates every hour when WiFi-connected
- **GitHub Integration**: Updates triggered by GitHub releases with firmware binaries

### Manual Updates
- Settings page has "Firmware Updates" section
- Click "Check for Updates" to manually trigger check
- Click "Install Latest Update" for immediate update

### GitHub Actions CI/CD
- **Automated Builds**: Every push triggers compilation and testing
- **Release Builds**: GitHub releases automatically include firmware binaries
- **Firmware Files**: `firmware-v1.0.0.bin`, `bootloader-v1.0.0.bin`, `partitions-v1.0.0.bin`

## Network Setup

### WiFi Access Point Mode (Default)
- **SSID**: `FridgeControl_[MAC]`
- **Password**: `fridge123`
- **Web Interface**: `http://fridge.local` or device IP
- **Direct IP**: Shown in Serial monitor

## Development Roadmap

✅ **Completed Features**:
- [x] Basic dual-zone control
- [x] Web interface
- [x] Temperature logging with charts
- [x] Multi-network WiFi support
- [x] GitHub OTA firmware updates

## License

This project is open source - use at your own risk. See individual files for licensing details.
