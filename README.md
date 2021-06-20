# 2021IoTIntro
IoT Getting Started - ESP32 with flashing LED

https://platformio.org/
How to install PlatformIO in VS Code - https://bit.ly/354Q0NV
How to get started - https://bit.ly/3ctJioK
<img width="536" alt="image" src="https://user-images.githubusercontent.com/54077762/122655652-4956b300-d111-11eb-9cd0-8f103fab9bd8.png">

## Sync code from Github
1. Clone GitHub repository
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

## Testing
Open **MQTT Test** and send the following messages:

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
