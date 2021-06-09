#include <secrets.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <string.h>

//Wifi and NTP Setup
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(1024);

// Light LED
#define LED_PIN 2

// ------------------------------------------------------------------------------------------------
// Function Declarations
// ------------------------------------------------------------------------------------------------

void connectToWiFi();
void connectToAwsIot();
void messageHandler(String &topic, String &payload);
void publishMessage();
//void shadowGet();
//void shadowPubMessage();
//int shadowSubscribeTopic(String topicName);
//void jsonAlarmState (char* outString, int timeStamp, float temperature, int humidity, int waterReading, bool alarmState);
//void jsonCreateTopicUpdate (char* outString, int timeStamp, float temperature, int humidity, int waterReading, int alarmState);
//void jsonCreateStateReported (char* outString, int timeStamp, float temperature, int humidity, int waterReading, int alarmState);
//void jsonCreateAlarm (char* outString, int timeStamp, float temperature, int humidity, int waterReading, int alarmState);


// ------------------------------------------------------------------------------------------------
// Setup Function
// ------------------------------------------------------------------------------------------------

void setup() {
  // Setup serial interface for ESP32
  Serial.begin(115200);

  //Initialize Light
  pinMode (LED_PIN, OUTPUT);

  // Initialized Wifi
  connectToWiFi();

  // Connect to AWS IoT Core
  connectToAwsIot();
  
  client.subscribe("$aws/things/esp_light2/shadow/update/delta");
  client.subscribe("Test123");


  // CHECK - Is this required?
  delay(100);        

  // publish to Classic Shadow /get to receive current configuration message.
  //shadowGet();
}

// ------------------------------------------------------------------------------------------------
// Loop Function
// ------------------------------------------------------------------------------------------------

void loop() {

  // Setup MQTT Client Loop
  client.loop();
  delay(1000);
}

// ------------------------------------------------------------------------------------------------
// connectToWifi Function
// ------------------------------------------------------------------------------------------------

void connectToWiFi() {
  Serial.print("INFO: Connecting to Wifi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Only try 15 times to connect to the WiFi
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 15){
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println("");
}

// ------------------------------------------------------------------------------------------------
// connectToAwsIot Function
// ------------------------------------------------------------------------------------------------

void connectToAwsIot()
{
  //Configure Wifi to use AWS IoT Certificates
  net.setCACert(aws_root_ca_pem);
  net.setCertificate(certificate_pem_crt);
  net.setPrivateKey(private_pem_key);
  
  // Connect to MQTT Broker
  client.begin(AWS_IOT_ENDPOINT, 8883, net);

  // Create Message Handler (Called when subscribed shadows updated)
  client.onMessage(messageHandler);

  Serial.print("INFO: Connecting to AWS IOT");
  while(!client.connect(DEVICE_NAME)) {
    Serial.print(".");
    delay(100);
  }

  if(!client.connected()) {
    Serial.println("");
    Serial.println("AWS IoT Timeout!");
    return;
  }

  Serial.println("");
  Serial.println("INFO: Connected to AWS IoT!");
}

// ------------------------------------------------------------------------------------------------
// Message Handler
// ------------------------------------------------------------------------------------------------

void messageHandler(String &topic, String &payload) {

  Serial.print("INFO: Starting Message Handler: ");
  Serial.println(topic.c_str());
  Serial.println("INFO: Payload: ");
  Serial.println(payload);


  StaticJsonDocument<1024> doc;
  deserializeJson(doc, payload);   // DOCUMENTATION HERE https://arduinojson.org/v6/doc/deserialization/

  if (doc["state"]["lightstatus"] == 1) {
    Serial.println("INFO: Turning on LED.");
    digitalWrite(LED_PIN, HIGH); //Turn On LED
  } else if (doc["state"]["lightstatus"] == 0) {
    Serial.println("INFO: Turning off LED.");
    digitalWrite(LED_PIN, LOW);
  }

}

// ------------------------------------------------------------------------------------------------
// Publish Message
// ------------------------------------------------------------------------------------------------

void publishMessage()
{
  //StaticJsonDocument<200> doc;
  //doc["time"] = millis();
  //doc["sensor_a0"] = analogRead(0);
  //char jsonBuffer[512];
  //serializeJson(doc, jsonBuffer); // print to client

  //client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
