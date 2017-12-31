# TempStore
Arduino Temperature Reader that stores information in DynamoDB.  This utilizes a WeMos D1 R2 board and DHT22 Temperature/Humidity sensor

Utilize this repository for the DynamoDB Code: https://github.com/brainstain/aws-sdk-arduino-esp8266 - Hopefully this will get merged into the Arduino library (Pull Request is generated).

## Directions
1) Replace *ssid and *password with your WiFi ssid and password
2) Replace TABLE_NAME with a name for your DynamoDB Table
3) Create a DynamoDB table in AWS, include a Hash Key Name of room and a sortable field time
4) Enter in your AWS_REGION, awsid as your user id, and awssecret as your user secret 
