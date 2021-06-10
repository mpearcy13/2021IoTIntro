#include <secrets.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include <string.h>

//Wifi and NTP Setup
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(1024);

// Light LED
#define LED_PIN 21

// Variables for IoT Topics to subscribe in Setup
char awsIotTopicUpdateDelta[128];
char awsIotTopicGetAccepted[128];

// Variables for IoT Topics to publish to
char awsIotTopicUpdate[128];
char awsIotTopicGet[128];

// ------------------------------------------------------------------------------------------------
// Function Declarations
// ------------------------------------------------------------------------------------------------

void connectToWiFi();
void connectToAwsIot();
void messageHandler(String &topic, String &payload);
void publishMessage(int fLightStatus);
void getIotState();

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
  
  // Subscribe to IoT Topics
  snprintf((awsIotTopicUpdateDelta), sizeof(awsIotTopicUpdateDelta), "%s%s%s", "$aws/things/", DEVICE_NAME, "/shadow/update/delta");
  client.subscribe(awsIotTopicUpdateDelta);
  
  snprintf(awsIotTopicGetAccepted, sizeof(awsIotTopicGetAccepted), "%s%s%s", "$aws/things/", DEVICE_NAME, "/shadow/get/accepted");
  client.subscribe(awsIotTopicGetAccepted);
  
  client.subscribe("Test123");

  // Setup IoT Topic for publishing.
  snprintf((awsIotTopicUpdate), sizeof(awsIotTopicUpdate), "%s%s%s", "$aws/things/", DEVICE_NAME, "/shadow/update");
  snprintf((awsIotTopicGet), sizeof(awsIotTopicGet), "%s%s%s", "$aws/things/", DEVICE_NAME, "/shadow/get");

  // CHECK - Is this required?
  //delay(100);   

  // publish to Classic Shadow /get to receive current configuration message.
  getIotState();
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

  Serial.print("INFO: Starting Message Handler.  Topic: ");
  Serial.println(topic.c_str());
  Serial.println("INFO: Payload: ");
  Serial.println(payload);

  // If Topic is from /Update/delta then use this code to parse JSON.
  if(strstr(topic.c_str(), "update/delta") != NULL) {
    // Parse JSON payload from IoT Core MQTT Queues
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);   // DOCUMENTATION HERE https://arduinojson.org/v6/doc/deserialization/

    if (doc["state"]["lightstatus"] == 1) {
      Serial.println("INFO: Turning on LED.");
      digitalWrite(LED_PIN, HIGH); //Turn On LED
      publishMessage(1);
    } else if (doc["state"]["lightstatus"] == 0) {
      Serial.println("INFO: Turning off LED.");
      digitalWrite(LED_PIN, LOW);
      publishMessage(0);
    }
  }
  // If topic is from /get/accepted this it will use this code to parse JSON.
  if(strstr(topic.c_str(), "get/accepted") != NULL) {
    // Parse JSON payload from IoT Core MQTT Queues
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);   // DOCUMENTATION HERE https://arduinojson.org/v6/doc/deserialization/

    if (doc["state"]["desired"]["lightstatus"] == 1) {
      Serial.println("INFO: Turning on LED.");
      digitalWrite(LED_PIN, HIGH); //Turn On LED
      publishMessage(1);
    } else if (doc["state"]["desired"]["lightstatus"] == 0) {
      Serial.println("INFO: Turning off LED.");
      digitalWrite(LED_PIN, LOW);
      publishMessage(0);
    }
  }
}

// ------------------------------------------------------------------------------------------------
// Publish Message
// ------------------------------------------------------------------------------------------------

void publishMessage(int fLightStatus)
{
  // Create JSON document to send back to AWS IOT Core.
  StaticJsonDocument<200> doc;
  JsonObject obj = doc.createNestedObject("state");
  obj = obj.createNestedObject("reported");
  obj["lightstatus"] = fLightStatus;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);

  client.publish(awsIotTopicUpdate, jsonBuffer);
}

void getIotState()
{
  //Serial.println("");
  Serial.println("\nINFO: Requesting status from Device Shadow.");
  
  StaticJsonDocument<200> doc;
  char jsonBuffer[128];
  serializeJson(doc, jsonBuffer);

    client.publish(awsIotTopicGet, jsonBuffer);
}