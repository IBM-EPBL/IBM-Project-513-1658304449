#include <WiFi.h>//library for wifi
#include <PubSubClient.h>//library for MQtt 
#define ECHO_PIN 13
#define TRIG_PIN 12
#include <ESP32Servo.h>
const int servoPin = 18;
int ledPin1=33; //definition digital 33 pins as pin to control the red LED
int ledPin2=25; //definition digital 25 pins as pin to control the green LED
int ledPin3=26; //definition digital 26 pins as pin to control the orange LED
Servo servo;
int pos = 0;
int weight;

void callback(char* subscribetopic, byte* payload, unsigned int payloadLength); 

//-------credentials of IBM Accounts------

#define ORG "ny6nkk"//IBM ORGANITION ID
#define DEVICE_TYPE "ESP32"//Device type mentioned in ibm watson IOT Platform
#define DEVICE_ID "smart_waste"//Device ID mentioned in ibm watson IOT Platform
#define TOKEN "12345678"     //Token
String data3;
float h, t;


//-------- Customise the above values --------
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";// Server Name
char publishTopic[] = "iot-2/evt/Data/fmt/json";// topic name and type of event perform and format in which data to be send
char subscribetopic[] = "iot-2/cmd/command/fmt/String";// cmd  REPRESENT command type AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";// authentication method
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;//client id


//-----------------------------------------
WiFiClient wifiClient; // creating the instance for wificlient
PubSubClient client(server, 1883, callback ,wifiClient); //calling the predefined client id by passing parameter like server id,portand wificredential



void setup()
{ Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ledPin1,OUTPUT);    //Set the digital 33 port mode, OUTPUT: Output mode
  pinMode(ledPin2,OUTPUT);    //Set the digital 25 port mode, OUTPUT: Output mode
  pinMode(ledPin3,OUTPUT);    //Set the digital 26 port mode, OUTPUT: Output mode
  servo.attach(servoPin, 500, 2400);
  digitalWrite(ledPin3,LOW);
  wificonnect();
  mqttconnect();
  
}



int readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  int duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
}

void loop() {
  weight=random()/100000000;
  Serial.print(" Weight of the Garbage in the bin(kg):");
  Serial.println(weight);
  int distance = readDistanceCM();
  if(distance < 50)
  {
    digitalWrite(ledPin1, HIGH);
    digitalWrite(ledPin2, LOW);
    Serial.print(" The Garbage Bin is FULL \n ");
    Serial.print("Garbage_level_in_bin:");
    Serial.println(400-distance);
    for (pos; pos <= 180; pos += 1)
   {
    servo.write(pos);
    delay(15);
   }
    
  }
else
  {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, HIGH);
    Serial.print(" The Garbage Bin is NOT FULL \n ");
    Serial.print("Garbage_level_in_bin(cm):");
    Serial.println(400-distance);
    for (pos ; pos >= 0; pos -= 1) 
    {
     servo.write(pos);
     delay(15);
    }
    
  }
  PublishData(weight,400-distance);
  delay(1000);
  if (!client.loop()) {
    mqttconnect();
  }
  delay(1000);
}

/*.....................................retrieving to Cloud...............................*/

void PublishData(int Weight, int Distance) {
  mqttconnect();//function call for connecting to ibm
  /*
     creating the String in in form JSon to update the data to ibm cloud
  */
  String payload = "{\"weight\":";
  payload += Weight;
  payload += "," "\"level\":";
  payload += Distance;
  payload += "}";

  
  Serial.print("Sending payload: ");
  Serial.println(payload);

  
  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish ok");// if it sucessfully upload data on the cloud then it will print publish ok in Serial monitor or else it will print publish failed
  } else {
    Serial.println("Publish failed");
  }
  
}


void mqttconnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting client to ");
    Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
      
     initManagedDevice();
     Serial.println();
  }
}
void wificonnect() //function defination for wificonnect
{
  Serial.println();
  Serial.print("Connecting to ");

  WiFi.begin("Wokwi-GUEST", "", 6);//passing the wifi credentials to establish the connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initManagedDevice() {
  if (client.subscribe(subscribetopic)) {
    Serial.println((subscribetopic));
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}

void callback(char* subscribetopic, byte* payload, unsigned int payloadLength) 
{
  
  Serial.print("callback invoked for topic: ");
  Serial.println(subscribetopic);
  for (int i = 0; i < payloadLength; i++) {
    //Serial.print((char)payload[i]);
    data3 += (char)payload[i];
  } 
  Serial.println("data: "+ data3); 
  if(data3=="lighton")
  {
digitalWrite(ledPin3,HIGH);
  }
  else 
  {
digitalWrite(ledPin3,LOW);
  }
data3="";  
}