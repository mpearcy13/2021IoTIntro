# 2021 IoT ESP32 using AWS IoT Shadows

IoT Getting Started - ESP32 with flashing LED

This project is an introduction to AWS IoT Core and using Classic Shadows to control the state of the device.  The device will use the shadow to turn on and off a LED connected to PIN 21 or the onboard LED using pin 2.


https://platformio.org/

How to get started - https://bit.ly/3ctJioK

How to install PlatformIO in VS Code - https://bit.ly/354Q0NV

## Sync code from Github
1. Clone GitHub repository
```
git clone https://github.com/mpearcy13/2021IoTIntro.git
```
2. Create a new project in PIO pointing to the directory cloned.

## Create Policy
1. Connect to AWS Console and open **IoT Core**
2. Open **Security** and select **Policies**
3. Create a new Policy

```
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": "iot:*",
      "Resource": "arn:aws:iot:**REGION**:**ACCOUNTNUMBER**:*"
    }
  ]
}
```

## Setup AWS IoT Thing
1.  Connect to AWS Console.
2.  Open **IoT Core**
4.  Open **Manager** then click on **Things**
5.  Create a new Thing (Single Thing)
6.  Create a certificate (One-click certificate creation)
7.  Download the certificates
    * A certificate for this thing
    * A private key
    * Root CA for AWS IoT
8. Select your Device Policy.

## Activate Certificate
1. Open Thing in console
2. Click on Security
3. Open Certificate
4. From the action menu, choose **Activate**

## Update Project and load your ESP32
1. Open the Project in Visual Studio
2. Open secrets.h
3. Update the "DEVICE_NAME" to match the Thing created above.
4. Update the "WIFI_SSID"
5. Update the "WIFI_PASSWORD"
6. Update the "AWS_IOT_ENDPOINT" (Available from IoT Core console under settings)
7. Copy the certificate for the device and copy to "certificate_pem_crt".  Add **\n\** to each line and **\n** to the last line.
8. Copy the private key to "private_pem_key".  Add **\n\** to each line and **\n** to the last line.
9. [OPTIONAL] in main.cpp change the "LED_PIN" to 2 if using the onboard ESP32 LED instead of an extneral LED.
10. Connect the ESP32 to the USB port on your system.
11. Click the PlatformIO Upload button ➡️ on the bottom bar.
12. When prompted in the console "Connecting....._____....._____" press the right hand button on the ESP32 to load the software.
13. Then client the PlatformIO Serial Monitor button 🔌 to view the serial output for the ESP32.

## Testing
Open **MQTT Test client** and send the following messages:

To turn off the light...
Topic: $aws/things/**DEVICENAME**/shadow/update
```
{
  "state": {
    "desired": {
      "lightstatus": 0
    }
  }
}
```

To turn on the light...
Topic: $aws/things/**DEVICENAME**/shadow/update
```
{
  "state": {
    "desired": {
      "lightstatus": 1
    }
  }
}
```

To get the thing's Device Shadows settings...
Subscribe to topics:
  1. $aws/things/**DEVICENAME**/shadow/get/accepted
  2. $aws/things/**DEVICENAME**/shadow/get/rejected

Send an empty JSON document to the following topic:
$aws/things/**DEVICENAME**/shadow/get

```
{}
```
