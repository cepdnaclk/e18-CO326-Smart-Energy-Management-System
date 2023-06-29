#include <WiFi.h>
// #include <WiFiClient.h>

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

EnergyMonitor emon_for_curr_sen_1;
EnergyMonitor emon_for_curr_sen_2;
EnergyMonitor emon_for_volt_sen;

// defines two calibration constants for the voltage 
// and current measurements
#define vCalibration 234.26
#define currCalibration 0.50
#define phaseShift 1.7

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

// get sensor reading from current sensor 1
// returns Irms as a float
float getCurrentSensor1Reading() {
  emon_for_curr_sen_1.current(UoP_CO326_Group15_Sensor_Curr_1, currCalibration);
  // float calIrms;
  // float curr1In;

  emon_for_curr_sen_1.calcVI(20,2000); // Calculate all. No.of wavelengths, time-out
  emon_for_curr_sen_1.serialprint(); // Print out all variables

  // digitalWrite(trigPinSensor1, LOW);
  // delayMicroseconds(2);
  // digitalWrite(trigPinSensor1, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(trigPinSensor1, LOW);

  // curr1In = pulseIn(echoPinSensor1, HIGH);

  return emon_for_curr_sen_1.Irms;
}

// get sensor reading from current sensor 2
// returns Irms as a float
float getCurrentSensor2Reading() {
  emon_for_curr_sen_2.current(UoP_CO326_Group15_Sensor_Curr_2, currCalibration);
  // float calIrms;
  // float curr2In;

  emon_for_curr_sen_2.calcVI(20,2000); // Calculate all. No.of wavelengths, time-out
  emon_for_curr_sen_2.serialprint(); // Print out all variables
  
  // digitalWrite(trigPinSensor2, LOW);
  // delayMicroseconds(2);
  // digitalWrite(trigPinSensor2, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(trigPinSensor2, LOW);
  
  // curr2In = pulseIn(echoPinSensor2, HIGH);

  return emon_for_curr_sen_2.Irms;
}

// get sensor reading from the voltage sensor
// returns Vrms as a float
float getVoltageSensorReading() {
  emon_for_volt_sen.voltage(UoP_CO326_Group15_Sensor_Volt, vCalibration, phaseShift);
  // float calVrms;
  // float VIn;

  emon_for_volt_sen.calcVI(20,2000); // Calculate all. No.of wavelengths, time-out
  emon_for_volt_sen.serialprint(); // Print out all variables
  
  // digitalWrite(trigPinSensor, LOW);
  // delayMicroseconds(2);
  // digitalWrite(trigPinSensor, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(trigPinSensor, LOW);
  
  // VIn = pulseIn(echoPinSensor, HIGH);

  return emon_for_volt_sen.Vrms;
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

  // set current sensors pins mode
  pinMode(trigPinSensor1, OUTPUT);
  pinMode(echoPinSensor1, INPUT);
  pinMode(trigPinSensor2, OUTPUT);
  pinMode(echoPinSensor2, INPUT);

  pinMode(UoP_CO326_Group15_Sensor_Curr_1, OUTPUT);
  pinMode(UoP_CO326_Group15_Sensor_Curr_2, OUTPUT);


  // // set voltage sensor pins mode
  // pinMode(trigPinSensor, OUTPUT);
  // pinMode(echoPinSensor, INPUT);

  pinMode(UoP_CO326_Group15_Sensor_Volt, OUTPUT);

  // connect to wifi
  setup_wifi();

  // set mqtt configurations
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // // For the Voltage Sensor: input pin, calibration, phase_shift
  // emon.voltage(UoP_CO326_Group15_Sensor_Volt, vCalibration, 1.7); 

  // // For Current Sensors: input pin, calibration.
  // emon.voltage(UoP_CO326_Group15_Sensor_Curr_1, currCalibration); 
  // emon.current(UoP_CO326_Group15_Sensor_Curr_2, currCalibration);   
}

void loop() {
  // if mqtt is not conected. connect to mqtt broker
  if (!client.connected()) {
    reconnect();
  }

  // get readings from the voltage sensor
  float Vrms = getCurrentSensor1Reading();

  // get readings from the current sensors
  float Irms1 = getCurrentSensor1Reading();
  float Irms2 = getCurrentSensor2Reading();

  float Iavg = (Irms1 + Irms2) / 2.0;

  String dateAndTime = getLocalTimeAsString();

  // create the mqtt msg to send
  String msg = "{\"deviceName\" : \"Group15Node1\" , \
    \"timeMeasured\" : \"" + dateAndTime + "\", \
         \"VoltageSensor\" : \""+String(Vrms)+"\", \
         \"CurrentSensor1\" : \""+String(Irms1)+"\", \
         \"CurrentSensor2\" : \""+String(Irms2)+"\"}";

  char msgCharAray[500] = {};

  msg.toCharArray(msgCharAray, 499);

  client.publish("group15/sensors", msgCharAray);
  client.loop();


  delay(1000);
}