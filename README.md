# Fridge_Control - ESP32 Dual Zone Fridge Controller with OTA Updates & Temperature Calibration

This ESP32-based project implements a dual-zone refrigerator controller with advanced web interface, temperature logging with interactive charts, **automatic over-the-air firmware updates** through GitHub, and **professional two-point temperature calibration** for precise NTC thermistor accuracy.

## Features

- **Dual Zone Control**: Independent temperature control for two separate cooling zones
- **Web Interface**: Responsive web UI accessible via WiFi for monitoring and configuration
- **Temperature Logging**: Real-time charts showing temperature history for both zones
- **OTA Firmware Updates**: Automatic updates through GitHub Actions (safe updates only when compressor is OFF)
- **NTC Temperature Calibration**: Two-point calibration system for accurate beta coefficient calculation and precise temperature readings
- **Automatic Control**: Intelligent cooling logic with configurable parameters (hysteresis, timing, setpoints)
- **Manual Override**: Full manual control of compressor and zone switching for maintenance/diagnosis
- **Temperature Sensing**: NTC thermistor sensors with custom calibration for accurate temperature monitoring
- **Relay Control**: 2-relay system for compressor and solenoid valve operation
- **System Status**: Real-time status monitoring and error handling
- **Configuration Storage**: Persistent settings stored in EEPROM (expanded for calibration data)
- **WiFi Access Point**: Built-in hotspot for initial setup and standalone operation
- **OTA Updates**: Over-the-air firmware updates via ArduinoOTA
- **Testing Mode**: Hardware-validated testing mode using potentiometer for temperature simulation
- **Multi-Network WiFi**: Support for up to 5 WiFi networks with automatic failover
- **GitHub Integration**: Automated CI/CD pipeline for firmware builds and releases

## Hardware Requirements

### Core Components
- **ESP32 Development Board** (any ESP32 board with WiFi)
- **2x Relay Modules** (5V coil, SPDT contacts rated for respective loads)
- **NTC Thermistors** (Auto calibrated in settings)
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

## OTA Firmware Updates 

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

## NTC Temperature Calibration (New!)

The controller features a sophisticated two-point calibration system for NTC thermistor sensors, providing much more accurate temperature readings than generic factory constants.

### Why Calibration Matters
- **Factory Defaults**: Standard NTC values provide approximate temperatures but can have 2-5°C error
- **Custom Calibration**: Calculates exact beta coefficient for your specific thermistors
- **Better Accuracy**: Provides precise temperature control for food preservation

### Calibration Process

1. **Point 1 - Room Temperature**:
   - Leave refrigerator door open so both NTC sensors reach room temperature
   - Navigate to Settings → NTC Temperature Calibration section
   - Enter your current room temperature (e.g., 25.0°C) in the "Point 1 Temp" field
   - Click "Set Point 1 (Room Temp)" - the system will record resistance values

2. **Point 2 - Cold Temperature**:
   - After at least 15 minutes, expose NTC sensors to a significantly different temperature
   - Options: Ice water (0°C), freezer compartment, or measured cold source
   - Enter the actual cold temperature in "Point 2 Temp" field
   - Click "Set Point 2 (Cold Temp)" - completes calibration automatically

### Automatic Calculations
The system calculates:
- **Beta Coefficient**: Using `B = ln(R₁/R₂) / (1/T₁ - 1/T₂)`
- **Nominal Resistance**: Reference resistance at calculated nominal temperature
- **Enhanced Accuracy**: Correct temperatures across full operating range (0°C to 30°C)

### Calibration Status
- **Active**: Shows calculated beta coefficient and nominal parameters
- **Default**: Falls back to standard NTC constants (β = 5000, nominal = 25°C)
- **Real-time Updates**: Web interface displays current calibration status

### Best Practices
- **Temperature Difference**: Ensure at least 10°C difference between point 1 and 2
- **Stable Conditions**: Allow temperatures to stabilize before recording points
- **Accurate Reference**: Use calibrated thermometer for reference temperatures
- **Periodic Recalibration**: Recalibrate yearly or if temperature accuracy seems off

### Technical Details
- **Formula**: Enhanced Steinhart-Hart equation with custom beta coefficient
- **EEPROM Storage**: Calibration data persists through power cycles
- **Fallback Protection**: Automatically uses defaults if calibration invalid
- **Sensor Matching**: Both NTC sensors calibrated together for consistency

## Development Roadmap

✅ **Completed Features**:
- [x] Basic dual-zone control
- [x] Web interface
- [x] Temperature logging with charts
- [x] Multi-network WiFi support
- [x] GitHub OTA firmware updates
- [x] NTC temperature calibration system

## License

This project is open source - use at your own risk. See individual files for licensing details.
