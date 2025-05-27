# ESP32 Fall Detection & Heart Rate Monitor

A comprehensive health monitoring system built with ESP32 that detects falls and monitors heart rate in real-time, with AWS IoT integration for remote monitoring and alerts.

## Features

- **Real-time Heart Rate Monitoring**: Continuous BPM tracking with MAX30105 pulse oximeter sensor
- **Fall Detection**: Accelerometer-based fall detection using MPU6050 sensor
- **AWS IoT Integration**: Secure MQTT communication with AWS IoT Core
- **Audio Alerts**: Buzzer notification system for detected falls
- **Health Interpretation**: Automatic heart rate classification (low, normal, elevated, high)
- **Wireless Connectivity**: WiFi-enabled for remote monitoring

## Hardware Requirements

### Core Components

- **ESP32 Development Board** (main microcontroller)
- **MAX30105 Pulse Oximeter Sensor** (heart rate monitoring)
- **MPU6050 Accelerometer/Gyroscope** (fall detection)
- **Buzzer** (audio alerts)

### Connections

| Component    | ESP32 Pin | Notes              |
| ------------ | --------- | ------------------ |
| MAX30105 SDA | GPIO 21   | I2C Data           |
| MAX30105 SCL | GPIO 22   | I2C Clock          |
| MPU6050 SDA  | GPIO 21   | I2C Data (shared)  |
| MPU6050 SCL  | GPIO 22   | I2C Clock (shared) |
| Buzzer       | GPIO 27   | Audio output       |
| VCC          | 3.3V      | Power supply       |
| GND          | GND       | Ground             |

## Software Dependencies

### Arduino Libraries

```cpp
#include <WiFiClientSecure.h>  // Secure WiFi connections
#include <PubSubClient.h>      // MQTT client
#include <ArduinoJson.h>       // JSON serialization
#include <WiFi.h>              // WiFi connectivity
#include <Wire.h>              // I2C communication
#include "MAX30105.h"          // Heart rate sensor
#include "heartRate.h"         // Heart rate algorithms
#include "MPU6050.h"           // Accelerometer/gyroscope
```

### Custom Files

- `secrets.h` - WiFi and AWS IoT credentials (see Configuration section)

## Configuration

### 1. Create `secrets.h` file

Create a `secrets.h` file in your project directory with the following structure:

```cpp
#define THINGNAME "your-iot-device-name"

const char WIFI_SSID[] = "your-wifi-ssid";
const char WIFI_PASSWORD[] = "your-wifi-password";
const char AWS_IOT_ENDPOINT[] = "your-aws-iot-endpoint.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
[Your Root CA Certificate]
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
[Your Device Certificate]
-----END CERTIFICATE-----
)EOF";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
[Your Private Key]
-----END RSA PRIVATE KEY-----
)EOF";
```

### 2. AWS IoT Setup

1. Create an AWS IoT Thing in the AWS Console
2. Generate device certificates and download them
3. Create and attach an IoT policy with appropriate permissions
4. Note your AWS IoT endpoint URL

### 3. MQTT Topics

- **Publish Topic**: `esp32/pub` (sensor data)
- **Subscribe Topic**: `esp32/sub` (commands/messages)

## System Operation

### Heart Rate Monitoring

- **Measurement Method**: Photoplethysmography (PPG) using MAX30105
- **Sampling**: Continuous monitoring with beat detection
- **Averaging**: Rolling average of last 4 readings for stability
- **Range**: Valid BPM range 20-255
- **Classifications**:
  - Low: < 50 BPM
  - Normal: 50-100 BPM
  - Slightly Elevated: 101-120 BPM
  - High: > 120 BPM

### Fall Detection Algorithm

1. **Impact Detection**: Monitors for acceleration > 2.5g
2. **Confirmation Period**: 300ms delay after impact
3. **Stillness Check**: Confirms fall if acceleration < 1g after delay
4. **Alert System**: Activates 1kHz buzzer tone upon fall detection

### Data Transmission

Publishes JSON data every ~5 seconds:

```json
{
  "beatAvg": 72,
  "fallDetect": false
}
```

## Installation & Setup

1. **Install Arduino IDE** with ESP32 board support
2. **Install Required Libraries** via Library Manager:

   - WiFiClientSecure
   - PubSubClient
   - ArduinoJson
   - MAX30105lib
   - MPU6050

3. **Hardware Assembly**:

   - Connect sensors according to the wiring diagram
   - Ensure stable power supply (USB or external)
   - Verify I2C connections

4. **Configuration**:

   - Create and populate `secrets.h`
   - Set up AWS IoT Thing and certificates
   - Configure WiFi credentials

5. **Upload Code**:
   - Open the main `.ino` file in Arduino IDE
   - Select ESP32 board and appropriate port
   - Compile and upload

## Usage Instructions

### Initial Setup

1. Power on the device
2. Monitor Serial output (115200 baud) for connection status
3. Wait for "AWS IoT Connected!" and "System ready." messages

### Heart Rate Monitoring

1. Place finger firmly on MAX30105 sensor
2. Keep finger still and ensure good contact
3. Wait for stable readings (indicated by "Normal resting heart rate" messages)
4. Remove finger when done - system will indicate "Keep your finger on the sensor"

### Fall Detection

- System automatically monitors for falls
- No user interaction required
- Buzzer will sound if fall is detected
- Fall status is transmitted to AWS IoT

## Troubleshooting

### Common Issues

**Sensor Not Found**

- Check I2C wiring (SDA/SCL connections)
- Verify power supply voltage (3.3V)
- Ensure sensors are properly seated

**WiFi Connection Failed**

- Verify SSID and password in `secrets.h`
- Check WiFi signal strength
- Ensure 2.4GHz network (ESP32 doesn't support 5GHz)

**AWS IoT Connection Issues**

- Verify certificates are correctly formatted
- Check IoT policy permissions
- Confirm endpoint URL is correct
- Ensure device is registered in AWS IoT

**Unstable Heart Rate Readings**

- Ensure firm finger contact with sensor
- Minimize movement during measurement
- Check for proper sensor placement
- Clean sensor surface if necessary

**False Fall Detection**

- Adjust acceleration thresholds if needed
- Ensure stable mounting of device
- Check for loose connections affecting MPU6050

### Serial Monitor Messages

- Monitor at 115200 baud for debugging information
- Key indicators: WiFi status, sensor readings, AWS connection status

## Technical Specifications

- **Microcontroller**: ESP32 (240MHz dual-core)
- **Heart Rate Accuracy**: ±2 BPM (with proper finger placement)
- **Fall Detection Sensitivity**: 2.5g impact threshold
- **Data Transmission**: Every ~5 seconds via MQTT
- **Power Consumption**: ~150mA during normal operation
- **Operating Voltage**: 3.3V
- **Communication**: WiFi 802.11 b/g/n

