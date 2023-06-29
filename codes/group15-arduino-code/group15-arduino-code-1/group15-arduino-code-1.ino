#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#include "EmonLib.h"  // for energy monitoring
    

// variables
const char *ssid = "Dialog 4G"; // Enter your WiFi name
const char *password = "M5DMH5F0G2M";  // Enter WiFi password
// MQTT Broker
const char *mqtt_broker = "test.mosquitto.org"; // Enter your WiFi or Ethernet IP
const char *topic_volt = "UoP_CO_326_E18_15_Voltage";
const char *topic_curr = "UoP_CO_326_E18_15_Current";
const char *topic_pow = "UoP_CO_326_E18_15_Power";
const char *topic_relay = "UoP_CO_326_E18_15_Relay";
const char *topic = "UoP/zzzzzCO/326/E18/19/";
const int mqtt_port = 1883;



//////////////////////////////
// pins of the current sensors
const int trigPinSensor1 = 5;
const int echoPinSensor1 = 18;

1Dint relayPin = D1; // Change D1 to the appropriate GPIO pin connected to the relay



// pins for the LEDs
#define UoP_CO326_Group15_Sensor_Curr_1 32
#define UoP_CO326_Group15_Sensor_Curr_2 25
#define UoP_CO326_Group15_Sensor_Volt 27

EnergyMonitor emon_for_curr_sen_1;
EnergyMonitor emon_for_curr_sen_2;
EnergyMonitor emon_for_volt_sen;

// defines two calibration constants for the voltage 
// and current measurements
#define vCalibration 234.26
#define currCalibration 0.50
#define phaseShift 1.7

WiFiClient espClient;
PubSubClient client(espClient);

 float voltage;

void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 delay(10);

//  dht.begin();
 
 pinMode(relayPin, OUTPUT);

 // connecting to a WiFi network
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.println("Connecting to WiFi..");
 }
 Serial.println("Connected to the WiFi network");
 
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback); 
 while (!client.connected()) {
  String client_id = "esp8266-client-";
  client_id += String(WiFi.macAddress()); 
  Serial.printf("The client %s connects to mosquitto mqtt broker\n", client_id.c_str()); 
  if (client.connect(client_id.c_str())) {
    Serial.println("Public emqx mqtt broker connected");
  } else {
    Serial.print("failed with state ");
    Serial.print(client.state());
    delay(2000);
 }
} 

 // publish and subscribe
 client.publish(topic, "Hello From ESP8266!");
//  client.publish(topic_volt);
 client.subscribe(topic_relay);
 
 
 
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 
 String messageTemp;
 for (int i = 0; i < length; i++) {
  messageTemp += (char)payload[i];
  Serial.print((char) payload[i]);
 }
 
 Serial.println(messageTemp);
 Serial.println();
 Serial.println(" - - - - - - - - - - - -");

 if (messageTemp == "on") {
  digitalWrite(relayPin, HIGH); // Turn on the relay
  Serial.println("Relay turned on");
} else if (messageTemp == "off") {
  digitalWrite(relayPin, LOW); // Turn off the relay
  Serial.println("Relay turned off");
}

}



void loop() {
 client.loop();
  publishDetails();
  
  delay(5000); // Adjust the delay as per your requirements
}





void publishDetails() {
  // get readings from the voltage sensor
  float Vrms = getCurrentSensor1Reading();
  char voltageStr[10];
  dtostrf(Vrms, 4, 2, voltageStr);

  float Irms1 = getCurrentSensor1Reading();
  char currentStr[10];
  dtostrf(Irms1, 4, 2, currentStr);

  char powerStr[10];
  dtostrf(Vrms*Irms1, 4, 2, powerStr);
  client.publish(topic_pow, powerStr);

  delay(1000);
  client.publish(topic_curr, currentStr);
delay(1000);

  client.publish(topic_volt, voltageStr);
  Serial.print("Voltage: ");
  Serial.println(voltageStr);
    Serial.print("Current: ");
  Serial.println(currentStr);
    Serial.print("Power: ");
  Serial.println(powerStr);
}

float getVoltageSensorReading() {
  emon_for_volt_sen.voltage(UoP_CO326_Group15_Sensor_Volt, vCalibration, phaseShift);
  // float calVrms;
  // float VIn;

  emon_for_volt_sen.calcVI(20,2000); // Calculate all. No.of wavelengths, time-out
  emon_for_volt_sen.serialprint(); // Print out all variables
  

  return emon_for_volt_sen.Vrms;
}


// get sensor reading from current sensor 1
// returns Irms as a float
float getCurrentSensor1Reading() {
  emon_for_curr_sen_1.current(UoP_CO326_Group15_Sensor_Curr_1, currCalibration);


  emon_for_curr_sen_1.calcVI(20,2000); // Calculate all. No.of wavelengths, time-out
  emon_for_curr_sen_1.serialprint(); // Print out all variables


  return emon_for_curr_sen_1.Irms;
}
