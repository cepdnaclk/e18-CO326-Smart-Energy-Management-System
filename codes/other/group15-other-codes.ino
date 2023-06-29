#include <WiFi.h>
#include <WiFiClient.h>

#include <PubSubClient.h>

#include "time.h"

#include "EmonLib.h"  // for energy monitoring
#include <EEPROM.h>  // for reading and writing data to 
                      // the ESP32’s non-volatile memory

// date and time settings
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;
const char* ntpServer = "pool.ntp.org";

// Replace the next variables with your SSID/Password combination
const char* ssid = "****";
const char* password = "******";

// Add your MQTT Broker IP address,
const char* mqtt_server = "test.mosquitto.org";

// mqtt client
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//////////////////////////////
// pins of the current sensors
const int trigPinSensor1 = 5;
const int echoPinSensor1 = 18;

const int trigPinSensor2 = 15;
const int echoPinSensor2 = 2;
//////////////////////////////

// pins of the voltage sensor
// const int trigPinSensor = 5;
// const int echoPinSensor = 18;

// pins for the LEDs
#define UoP_CO326_Group15_Sensor_Curr_1 32
#define UoP_CO326_Group15_Sensor_Curr_2 25
#define UoP_CO326_Group15_Sensor_Volt 27

EnergyMonitor emon;

// defines two calibration constants for the voltage 
// and current measurements
#define vCalibration 83.3
#define currCalibration 0.50

// callback for MQTT
// This runs when there is a new message from MQTT
void callback(char* topic, byte* message, unsigned int length) {
  // print the message for debugging
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  // convert the byte messge into a char array
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

}

// conect to wifi
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// if MQTT is not connected. Connect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("co326_group15_smart_energy_management_system_node_1")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("group15/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// get sensor reading from sensor 1
// returns a float as the distance in centimeters
float getCurrentSensor1Reading() {
  long duration;
  float distanceCm;
  digitalWrite(trigPinSensor1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinSensor1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSensor1, LOW);
  duration = pulseIn(echoPinSensor1, HIGH);
  // distanceCm = duration * SOUND_SPEED / 2;
  // Serial.print("Ultransonic Sensor 1 Distance (cm): ");
  // Serial.println(distanceCm);

  return distanceCm;
}

// get sensor reading from sensor 2
// returns a float as the distance in centimeters
float getCurrentSensor2Reading() {
  long duration;
  float distanceCm;
  digitalWrite(trigPinSensor2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinSensor2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinSensor2, LOW);
  duration = pulseIn(echoPinSensor2, HIGH);
  // distanceCm = duration * SOUND_SPEED / 2;
  // Serial.print("Ultransonic Sensor 2 Distance (cm): ");
  // Serial.println(distanceCm);

  return distanceCm;
}


String getLocalTimeAsString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return "Failed to obtain time";
  }

  char buffer[150];
  strftime(buffer, sizeof(buffer), "%B %d %Y %H:%M:%S", &timeinfo);

  return String(buffer);
}

// runs this code once when program starts
void setup() {
  // set serial port baud rate
  Serial.begin(115200);

  // set ultrasoinc sensor pins mode
  pinMode(trigPinSensor1, OUTPUT);
  pinMode(echoPinSensor1, INPUT);
  pinMode(trigPinSensor2, OUTPUT);
  pinMode(echoPinSensor2, INPUT);

  // set LED pings
  pinMode(UoP_CO326_Group15_Sensor_Curr_1, OUTPUT);
  pinMode(UoP_CO326_Group15_Sensor_Curr_2, OUTPUT);
  pinMode(UoP_CO326_Group15_Sensor_Volt, OUTPUT);

  // connect to wifi
  setup_wifi();

  // set mqtt configurations
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

// run this code in a loop
void loop() {
  // if mqtt is not conected. connect to mqtt broker
  if (!client.connected()) {
    reconnect();
  }

  // get readings from the sensors
  int sensor1 = getCurrentSensor1Reading();
  int sensor2 = getCurrentSensor2Reading();

  String dateAndTime = getLocalTimeAsString();

  // create the mqtt msg to send
  String msg = "{\"deviceName\" : \"node1\" , \
    \"timeMeasured\" : \"" + dateAndTime + "\", \
         \"distanceSensor1\" : \""+String(sensor1)+"\", \
         \"distanceSensor2\" : \""+String(sensor2)+"\"}";

  char msgCharAray[500] = {};
msg.toCharArray(msgCharAray, 499);



client.publish("group15/sensors", msgCharAray);

client.loop();


delay(1000);
}