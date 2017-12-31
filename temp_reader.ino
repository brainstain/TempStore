#include <DHT.h>

#include <ESP8266WiFi.h>
#include "AWSFoundationalTypes.h"
#include "AmazonDynamoDBClient.h"
#include "ESP8266AWSImplementations.h"

#include "DHT.h"
 
#define DHTPIN D6     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)


Esp8266HttpClient httpClient;
Esp8266DateTimeProvider dateTimeProvider;
AmazonDynamoDBClient ddbClient;
ActionError actionError;
PutItemInput putItemInput;
char *ssid="your_ssid";
char *password="your_network_password";
char tres[5] = "";
char hres[5] = "";        

/* Contants describing DynamoDB table and values being used. */
static const char* HASH_KEY_NAME = "room";
static const char* HASH_KEY_VALUE = "room_a";
static const char* RANGE_KEY_NAME = "time";
const char* TABLE_NAME = "Dynamo_Table_Name";
/* Constants for connecting to DynamoDB. */
const char* AWS_REGION = "us-west-2";
const char* AWS_ENDPOINT = "amazonaws.com";
/* AWS Keys */
char *awsid="ID_KEY";
char *awssecret="AWS_SECRET";

DHT dht(DHTPIN, DHTTYPE);
 
void setup() {
  Serial.begin(9600); 
  dht.begin();

  // Connect to WAP
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

    ddbClient.setAWSRegion(AWS_REGION);
    ddbClient.setAWSEndpoint(AWS_ENDPOINT);
    ddbClient.setAWSSecretKey(awssecret);
    ddbClient.setAWSKeyID(awsid);
    ddbClient.setHttpClient(&httpClient);
    ddbClient.setDateTimeProvider(&dateTimeProvider);
}
 
void loop() {
  //Read every 5 minutes. 300000
  delay(300000);
 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature(false, false);
  t = dht.convertCtoF(t);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  else {
        /* Create an Item. */
        AttributeValue deviceValue;
        deviceValue.setS(HASH_KEY_VALUE);
        AttributeValue timeValue;
        /* Getting current time for Time attribute. */
        timeValue.setS(dateTimeProvider.getDateTime());
        AttributeValue tempValue;
        
        dtostrf(t,3,1,tres);
        tempValue.setS(tres);
        AttributeValue humidValue;
        dtostrf(h,3,1,hres);
        humidValue.setS(hres);
        MinimalKeyValuePair < MinimalString, AttributeValue
                > att1(HASH_KEY_NAME, deviceValue);
        MinimalKeyValuePair < MinimalString, AttributeValue
                > att2(RANGE_KEY_NAME, timeValue);
        MinimalKeyValuePair < MinimalString, AttributeValue> att3("TEMP", tempValue);
        MinimalKeyValuePair < MinimalString, AttributeValue> att4("HUMIDITY", humidValue);
        MinimalKeyValuePair<MinimalString, AttributeValue> itemArray[] = { att1,
                att2, att3, att4};

        /* Set values for putItemInput. */
        putItemInput.setItem(MinimalMap < AttributeValue > (itemArray, 4));
        putItemInput.setTableName(TABLE_NAME);

        /* perform putItem and check for errors. */
        Serial.println("Start Put Item");
        PutItemOutput putItemOutput = ddbClient.putItem(putItemInput,
                actionError);
        Serial.println("Finish Put Item");
        switch (actionError) {
        case NONE_ACTIONERROR:
            Serial.println("PutItem succeeded!");
            break;
        case INVALID_REQUEST_ACTIONERROR:
            Serial.print("ERROR: ");
            Serial.println(putItemOutput.getErrorMessage().getCStr());
            break;
        case MISSING_REQUIRED_ARGS_ACTIONERROR:
            Serial.println(
                    "ERROR: Required arguments were not set for PutItemInput");
            break;
        case RESPONSE_PARSING_ACTIONERROR:
            Serial.println("ERROR: Problem parsing http response of PutItem");
            break;
        case CONNECTION_ACTIONERROR:
            Serial.println("ERROR: Connection problem");
            break;
        }
        /* wait to not double-record */
        delay(2000);
    }
  
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *F ");
 
}
