# 12V Dual Zone Fridge ESP32 Controller

This project implements a dual-zone refrigerator controller using an ESP32 microcontroller with web interface for monitoring and control. Designed for standard 12V DC refrigerator systems.

## Features

- **Dual Zone Control**: Independent temperature control for two separate cooling zones
- **Web Interface**: Responsive web UI accessible via WiFi for monitoring and configuration
- **Automatic Control**: Intelligent cooling logic with configurable parameters (hysteresis, timing, setpoints)
- **Manual Override**: Full manual control of compressor and zone switching for maintenance/diagnosis
- **Temperature Sensing**: NTC thermistor sensors for accurate temperature monitoring
- **Relay Control**: 2-relay system for compressor and solenoid valve operation
- **System Status**: Real-time status monitoring and error handling
- **Configuration Storage**: Persistent settings stored in EEPROM
- **WiFi Access Point**: Built-in hotspot for initial setup and standalone operation
- **OTA Updates**: Over-the-air firmware updates via ArduinoOTA
- **Testing Mode**: Hardware-validated testing mode using potentiometer for temperature simulation

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

2. **Clone/Navigate to project**:
   ```bash
   cd dual-zone-fridge-project
   ```

3. **Build the project**:
   ```bash
   python -m platformio run
   ```

4. **Upload firmware**:
   ```bash
   python -m platformio run --target upload
   ```

5. **Upload web files to SPIFFS**:
   ```bash
   python -m platformio run --target uploadfs
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

## Network Setup

### WiFi Access Point Mode (Default)
- **SSID**: FridgeControl
- **Password**: 12345678
- **Web Interface**: http://fridge.local or http://192.168.4.1
- **mDNS**: http://fridge.local (if DNS works on your network)

### Station Mode (Optional)
Configure WiFi credentials in code:
```cpp
WiFi.begin("YourWiFiSSID", "YourWiFiPassword");
```

## Configuration Parameters

### Temperature Control
- **Setpoint Range**: -20°C to +10°C for both zones
- **Hysteresis**: 1.0-8.0°C temperature band before cooling activates
- **Temperature Offset**: Calibration correction for sensor accuracy

### Timing Parameters
- **Min Run Time**: 1-10 minutes minimum compressor on time
- **Min Stop Time**: 1-15 minutes minimum compressor off time
- **Min Zone Switch Time**: 1-10 minutes minimum time between zone changes
- **Max Run Time**: 10-45 minutes maximum continuous compressor runtime

## Testing and Safety

### Hardware Testing
1. **Power sequencing**: Connect 5V supply then 12V supply (verify polarity!)
2. **Signal verification**: Use multimeter to verify relay coil voltage (5V) and ESP32 outputs
3. **Load testing**: Verify solenoid movement and compressor startup (listen/feel for operation)
4. **Current measurement**: Use multimeter in series to verify solenoid current (0.5-1A) and compressor current (<5A)
5. **Safety checks**: Verify all 12V DC connections, fuses, and proper wire gauge before powering on

### Software Testing Mode
Enable `TESTING_MODE` in main.cpp:
```cpp
#define TESTING_MODE true  // Use potentiometer on pin 33 for simulation
```

### Safety Precautions (12V DC System)
- **DC Power Warning**: Compressor relay switches 12V DC - ensure proper insulation and correct polarity
- **Short Circuit Protection**: Use properly rated fuses (3-5A) in 12V supply to prevent fire
- **Polarity Protection**: Use diodes in series with solenoid loads if reverse polarity is a concern
- **Thermal Protection**: Monitor component temperatures during operation (relays can heat up)
- **Wire Gauge**: Use adequate wire size: 14-16 AWG for power circuits, 22-24 AWG for signal
- **Current Limiting**: ESP32 outputs are current-limited; verify relay trigger current requirements
- **Ground Fault Protection**: Maintain clean, corrosion-free connections to prevent voltage drops

## Troubleshooting

### Common Issues
- **No relay operation**: Check 5V supply voltage and ESP32 output voltage
- **WiFi connection failure**: Verify SSID/password in ESP32 configuration
- **Temperature inaccuracies**: Calibrate NTC sensors and check voltage divider values
- **Compressor short cycling**: Adjust min run/stop times and hysteresis
- **Web interface not loading**: Clear browser cache or use incognito mode

### Debug Output
Serial monitor output includes:
- Temperature readings for both zones
- System status and control decisions
- Error conditions and timing information
- Testing mode indicators

## Regulatory Compliance

This project implements safety features compliant with general appliance control standards:
- Temperature control accuracy within ±1°C
- Minimum cycle times prevent compressor stress
- Error detection prevents runaway conditions
- Manual override capability for service

**Note**: While this controller meets technical standards for DIY projects, it should not be used in commercial or medical refrigeration without additional certification and testing.

## Development Roadmap

Future enhancements may include:
- Temperature logging with data export
- Mobile app companion
- Advanced algorithms for better efficiency
- Cloud connectivity options
- Energy consumption monitoring
- Multiple compressor support
