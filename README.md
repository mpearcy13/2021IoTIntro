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
2. Open a project in PIO pointing to the directory cloned.

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

This policy will **Allow** access to **All IoT** services.


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
8. Select your Device Policy that was created in previous step.

## Activate Certificate
1. Open **Thing** under Manage in **IoT Core** console
2. Click on **Security**
3. Open the assigned certificate
4. From the action menu, choose **Activate**

## Update Project and load your ESP32
1. Open the Project in Visual Studio
  1. Click **PlatformIO** in the left menu, then click **Open** under **PIO Home**
  2. Click on **Open Project**
  3. Browse to the directory containing the synced Github files.
3. Open **secrets.h** in the **include** directory.
4. Update the **DEVICE_NAME** to match the Thing created above.
5. Update the **WIFI_SSID**
6. Update the **WIFI_PASSWORD**
7. Update the **AWS_IOT_ENDPOINT** (Available from IoT Core console under settings)
8. Open the certificate for the device and copy & paste to **certificate_pem_crt**.  Add **\n\** to each line and **\n** to the last line.
9. Open the private key and copy & paste to **private_pem_key**.  Add **\n\** to each line and **\n** to the last line.
10. [OPTIONAL] in **main.cpp** change the **LED_PIN** to **2** if using the onboard ESP32 LED instead of an extneral LED.
11. Connect the ESP32 to the USB port on your system.
12. Click the PlatformIO Upload button ➡️ on the bottom bar.
13. When prompted in the console "Connecting....._____....._____" press the right hand button on the ESP32 to load the software.
14. Then client the PlatformIO Serial Monitor button 🔌 to view the serial output for the ESP32.

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
