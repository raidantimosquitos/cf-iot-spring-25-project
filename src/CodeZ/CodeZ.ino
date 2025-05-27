#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "MPU6050.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

#define BUZZER_PIN 27

const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateIndex = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg = 0;
uint8_t count = 0;
bool isFall = false;
bool buzzing = false;
unsigned long buzzStartTime = 0;

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

MAX30105 particleSensor;
MPU6050 mpu;

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["beatAvg"] = beatAvg;
  doc["fallDetect"] = isFall;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectAWS();

  Wire.begin(21, 22);
  Wire.setClock(100000);

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found. Check wiring.");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (1);
  }

  Serial.println("System ready.");
}

void loop() {
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue)) {
    long now = millis();
    long delta = now - lastBeat;
    lastBeat = now;

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateIndex++] = (byte)beatsPerMinute;
      rateIndex %= RATE_SIZE;

      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  if (count == 250) {
    count = 0;

    if (irValue < 50000 || beatAvg == 0) {
      beatAvg = 0;
      Serial.print("Avg BPM: ");
      Serial.print(beatAvg);
      Serial.println(" - Keep your finger on the sensor.");
    } else {
      Serial.print("Avg BPM: ");
      Serial.print(beatAvg);
      interpretHeartRate(beatAvg);
    }

    publishMessage();
    client.loop();

    // if (buzzing && (millis() - buzzStartTime >= 1000)) {
    //   noTone(BUZZER_PIN);
    //   buzzing = false;
    // }
  }

  count++;

  detectFall();

  delay(20);
}

void interpretHeartRate(int bpm) {
  if (bpm < 50) {
    Serial.println(" - Low heart rate.");
  } else if (bpm <= 100) {
    Serial.println(" - Normal resting heart rate.");
  } else if (bpm <= 120) {
    Serial.println(" - Slightly elevated heart rate.");
  } else {
    Serial.println(" - High heart rate.");
  }
}

void detectFall() {
  static bool impactDetected = false;
  static unsigned long impactTime = 0;

  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  float accX = ax / 16384.0;
  float accY = ay / 16384.0;
  float accZ = az / 16384.0;

  float totalAccel = sqrt(accX * accX + accY * accY + accZ * accZ);

  if (!impactDetected) {
    if (totalAccel > 2.5) {
      impactDetected = true;
      impactTime = millis();
      Serial.print("Impact detected! Accel: ");
      Serial.println(totalAccel);
    }
  } else {
    if (millis() - impactTime > 300) {
      mpu.getAcceleration(&ax, &ay, &az);
      accX = ax / 16384.0;
      accY = ay / 16384.0;
      accZ = az / 16384.0;
      totalAccel = sqrt(accX * accX + accY * accY + accZ * accZ);

      if (totalAccel < 1) {
        Serial.println("⚠️ Possible fall detected!");
        isFall = true;

        if (!buzzing) {
          Serial.println("Activating buzzer (tone)!");
          tone(BUZZER_PIN, 1000); // 1kHz tone
          buzzStartTime = millis();
          buzzing = true;
        }
      }

      impactDetected = false;
    }
  }
}