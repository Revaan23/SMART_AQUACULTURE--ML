#include <SoftwareSerial.h>
#include <DHT.h>

// ------------------- SENSOR PIN SETUP -------------------
#define DHTPIN 2          // DHT11 signal pin connected to D2
#define DHTTYPE DHT11     // DHT11 sensor
#define GAS_PIN A1        // MQ-2 / MQ-135 Air quality sensor analog output
#define RX_PIN 6          // pH sensor TX → Arduino RX (D6)
#define TX_PIN 7          // pH sensor RX → Arduino TX (D7)

#define GREEN_LED 8       // ✅ ADD LED
#define RED_LED 9         // ✅ ADD LED

// ------------------- INITIALIZE OBJECTS -------------------
SoftwareSerial phSerial(RX_PIN, TX_PIN); 
DHT dht(DHTPIN, DHTTYPE);

// ------------------- VARIABLES -------------------
String phData = "";
bool phStringComplete = false;

void setup() {
  Serial.begin(9600);      // Serial monitor
  phSerial.begin(9600);    // pH sensor serial
  dht.begin();

  // ✅ LED PIN SETUP
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  Serial.println("=================================");
  Serial.println("💧 Smart Fish Tank Monitoring System");
  Serial.println("Sensors: DHT11 | pH | Air Quality | LED Status");
  Serial.println("=================================");
  delay(2000);
}

void loop() {
  // ------------------- DHT11 SENSOR -------------------
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("⚠️ DHT11 sensor read failed!");
  } else {
    Serial.print("🌡️ Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C  |  💧 Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // ------------------- GAS SENSOR -------------------
  int gasValue = analogRead(GAS_PIN);
  Serial.print("🌫️ Air Quality (Analog): ");
  Serial.println(gasValue);

  // ------------------- pH SENSOR -------------------
  while (phSerial.available()) {
    char inChar = (char)phSerial.read();
    if (inChar == '\r' || inChar == '\n') {
      phStringComplete = true;
    } else {
      phData += inChar;
    }
  }

  if (phStringComplete) {
    Serial.print("⚗️ pH Sensor Raw Data: ");
    Serial.println(phData);
    phData = "";
    phStringComplete = false;
  }

  // ----------------------------------------------------
  // ✅ RECEIVE SAFE / NOTSAFE FROM PYTHON FOR LED CONTROL
  // ----------------------------------------------------
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "SAFE") {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      Serial.println("Arduino: SAFE -> GREEN LED ON");
    } 
    else if (cmd == "NOTSAFE") {
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
      Serial.println("Arduino: NOT SAFE -> RED LED ON");
    }
  }

  Serial.println("---------------------------------");
  delay(2000);
}
