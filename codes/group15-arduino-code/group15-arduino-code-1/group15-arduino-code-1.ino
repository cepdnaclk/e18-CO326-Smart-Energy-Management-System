// #include <ESP8266WiFi.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "EmonLib.h"  // for energy monitoring

// WiFi settings
const char* ssid = "Dialog 4G"; // Enter your WiFi name
const char* password = "M5DMH5F0G2M"; // Enter WiFi password

// MQTT Broker settings
const char* mqtt_broker = "test.mosquitto.org"; // Enter your MQTT broker address
const int mqtt_port = 1883;

// MQTT topics
// const char* topic_volt = "UoP_CO_326_E18_15_Voltage";
// const char* topic_curr = "UoP_CO_326_E18_15_Current";
// const char* topic_pow = "UoP_CO_326_E18_15_Power";
// const char* topic_relay = "UoP_CO_326_E18_15_Relay";
// const char* topic = "UoP/zzzzzCO/326/E18/19/";

// Pins
const int relayPin = 18; // GPIO23

// Analog input pins for current sensor
const int analogPinL1 = A0; // Analog input pin for L1
const int analogPinL2 = 1; // Analog input pin for L2

// Pins of the voltage sensor
const int voltageSensorPin = 4;



WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");

  // Connect to MQTT broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  connectToMQTTBroker();

  // Publish and subscribe
  // client.publish(topic, "Hello From ESP8266!");
  client.subscribe("UoP_CO_326_E18_15_Relay");
}

void loop() {
  client.loop();
  publishDetails();
  // digitalWrite(relayPin, LOW); 
  delay(5000); // Adjust the delay as per your requirements
  // digitalWrite(relayPin, HIGH); 
  
}

void publishDetails() {
  float Vrms = getVoltageSensorReading();
  float Irms1 = getCurrentSensor1Reading();
  float power = Vrms * Irms1;

  String voltageStr = String(Vrms, 2);
  String currentStr = String(Irms1, 2);
  String powerStr = String(power, 2);

  publishWithDelay("UoP_CO_326_E18_15_Voltage", voltageStr.c_str(), 1000);
  publishWithDelay("UoP_CO_326_E18_15_Current", currentStr.c_str(), 1000);
  publishWithDelay("UoP_CO_326_E18_15_Power", powerStr.c_str(), 0); // No delay for immediate publishing

  Serial.print("Voltage: ");
  Serial.println(voltageStr);
  Serial.print("Current: ");
  Serial.println(currentStr);
  Serial.print("Power: ");
  Serial.println(powerStr);
}

float getCurrentSensor1Reading() {
  EnergyMonitor emon_for_curr_sen_1; // EnergyMonitor object created locally
  emon_for_curr_sen_1.current(analogPinL1, 60.6); // Calibration value may vary, adjust as per your sensor
  emon_for_curr_sen_1.calcVI(20, 2000); // Calculate all. No. of wavelengths, time-out
  emon_for_curr_sen_1.serialprint(); // Print out all variables
  return emon_for_curr_sen_1.Irms;
}

float getVoltageSensorReading() {
  int sensorValue = analogRead(voltageSensorPin);
  float voltage = (sensorValue / 1023.0) * 3.3; // Assuming 3.3V reference voltage
  return voltage;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");

  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)payload[i];
    Serial.print((char)payload[i]);
  }

  Serial.println(messageTemp);
  Serial.println();

  if (messageTemp == "on") {
    digitalWrite(relayPin, LOW); // Turn on the relay
    Serial.println("Relay turned on");
  } else if (messageTemp == "off") {
    digitalWrite(relayPin, HIGH); // Turn off the relay
    Serial.println("Relay turned off");
  }
}

void connectToMQTTBroker() {
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to mosquitto MQTT broker\n", client_id.c_str());
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx MQTT broker connected");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void publishWithDelay(const char* topic, const char* payload, unsigned int delayTime) {
  client.publish(topic, payload);
  delay(delayTime);
}
