#include <PubSubClient.h>
#include <WiFi.h> // Include the WiFi library for ESP32
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include <Wire.h> // Include the Wire library for I2C communication
#include <Adafruit_Sensor.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESP32Servo.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiSTA.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiType.h>
#include <ThingSpeak.h>
#include <WiFiUdp.h>
#define BUILTIN_LED 2
 // Include the BMP280 sensor library
#define BMP_SDA 21 // Define the ESP32 SDA pin
#define BMP_SCL 22 // Define the ESP32 SCL pin
Adafruit_BMP280 bmp; // Create a BMP280 sensor object

#define fanPWMPin 32
#define fanDirection 15

#define DHTTYPE DHT11
#define dht_dpin1 27
#define dht_dpin2 26
#define led_red 13
#define led_green 14

Servo myServo;  // Create a servo object

#define SEALEVELPRESSURE_HPA (1013.25)
DHT dht1(dht_dpin1, DHTTYPE);
DHT dht2(dht_dpin2, DHTTYPE);

// Ensure correct credentials to connect to your WiFi Network.
const char* ssid = "NO";
const char* pass = "Aditya@123";
// Ensure that the credentials here allow you to publish and subscribe to the ThingSpeak channel.
#define channelID 2469286
const char mqttUserName[] = "FgoSDAsmIhk2BzU5IhgQNgU"; 
const char clientID[] = "FgoSDAsmIhk2BzU5IhgQNgU";
const char mqttPass[] = "KgD6BlO7g6J0VaMxc1KbGwmw";

#define mqttPort 1883
WiFiClient client;
const char* server = "mqtt3.thingspeak.com";
int status = WL_IDLE_STATUS; 
long lastPublishMillis = 0;
int connectionDelay = 1;
int updateInterval = 15;
PubSubClient mqttClient( client );

// Function to handle messages from MQTT subscription.
void mqttSubscriptionCallback( char* topic, byte* payload, unsigned int length ) {
  // Print the details of the message that was received to the serial monitor.
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println(""); 
  char c = (char)(payload[168]);
  Serial.print("c=");
  Serial.println(c);
  if(c == '1') {
    analogWrite(fanPWMPin,120);
    digitalWrite(led_green,HIGH);
  }
  else {
    analogWrite(fanPWMPin,0);
    digitalWrite(led_red,HIGH);
    
  }
  Serial.println();
}

// Subscribe to ThingSpeak channel for updates.
void mqttSubscribe( long subChannelID ){
  String myTopic = "channels/"+String( subChannelID )+"/subscribe";
  mqttClient.subscribe(myTopic.c_str());
}

// Connect to WiFi.
void connectWifi()
{
  Serial.print( "Connecting to Wi-Fi..." );
  // Loop until WiFi connection is successful
  while ( WiFi.status() != WL_CONNECTED ) {
    WiFi.begin( ssid, pass );
    delay( connectionDelay*1000 );
    Serial.print( WiFi.status() ); 
  }
  Serial.println( "Connected to Wi-Fi." );
}

// Connect to MQTT server.
void mqttConnect() {
  // Loop until connected.
  while ( !mqttClient.connected() )
  {
    // Connect to the MQTT broker.
    if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) ) {
      Serial.println("Connected to MQTT broker...");
      Serial.print( "MQTT to " );
      Serial.print( server );
      Serial.print (" at port ");
      Serial.print( mqttPort );
      Serial.println( " successful." );
    } else {
      Serial.print( "MQTT connection failed, rc = " );
      Serial.print( mqttClient.state() );
      Serial.println( " Will try again in a few seconds" );
      delay( connectionDelay*1000 );
    }
  }
}
// Publish messages to a ThingSpeak channel.
void mqttPublish(long pubChannelID, String message) {
  String topicString ="channels/" + String( pubChannelID ) + "/publish";
  mqttClient.publish( topicString.c_str(), message.c_str() );
}

void setup() {
  Serial.begin( 115200 );
  delay(3000);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(fanPWMPin, OUTPUT);
  digitalWrite(fanDirection, HIGH);
   digitalWrite(BUILTIN_LED, HIGH);
  connectWifi();
  mqttClient.setServer(server, mqttPort); 
  mqttClient.setCallback(mqttSubscriptionCallback);
  mqttClient.setBufferSize(2048);
  // Configure the MQTT client
    dht1.begin();
      dht2.begin();
     if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  pinMode(led_red,OUTPUT);
  pinMode(led_green,OUTPUT);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, LOW);
  myServo.attach(23); // Attach the servo motor to pin 23
  // myServo.write(0);
  // Set the MQTT message handler function.
  // mqttClient.setCallback( mqttSubscriptionCallback );
  // Set the buffer to handle the returned JSON. NOTE: A buffer overflow of the message buffer will result in your callback not being invoked.
  // mqttClient.setBufferSize( 2048 );
}

void loop() {

  // Reconnect to WiFi if it gets disconnected.
  if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
  }
  if (!mqttClient.connected()) {
     mqttConnect(); 
     mqttSubscribe( channelID );
  }
  mqttClient.loop(); 
  float temperature1 = dht1.readTemperature();
  float humidity1 = dht1.readHumidity();
  float temperature2 = dht2.readTemperature();
  float humidity2 = dht2.readHumidity();
  float pressure = bmp.readPressure(); // Read pressure in Pa
    // digitalWrite(led_red,HIGH);
 float temp =(temperature1+temperature2)/2;
  float humidity =(humidity1+humidity2)/2;

  if (temp > 34 ||(temp > 34 && humidity > 45 )|| pressure/100.0 > 950) {
    analogWrite(fanPWMPin,255);
    myServo.write(90); // Rotate servo to 180 degrees
    digitalWrite(led_green,LOW);
    delay(100);
    digitalWrite(led_red,HIGH);

  } else {
    analogWrite(fanPWMPin,30);
    myServo.write(0); // Rotate servo back to 0 degrees
    digitalWrite(led_red,LOW);
    delay(100);
    digitalWrite(led_green,HIGH);

  }
  Serial.print("Temperature Avg.: ");
  Serial.print(temp);
  Serial.print(" °C ,");
  Serial.print(" ");
  Serial.print("Humidity Avg.: ");
  Serial.print(humidity);
  Serial.print("Temperature1: ");
  Serial.print(temperature1);
  Serial.print(" °C ,");
  Serial.print(" ");
  Serial.print("Humidity1: ");
  Serial.print(humidity1);
  Serial.println(" %");
   Serial.print("Temperature2: ");
  Serial.print(temperature2);
  Serial.print(" °C ,");
  Serial.print(" ");
  Serial.print("Humidity2: ");
  Serial.print(humidity2);
  Serial.print(" %");
  Serial.print(" °C, Pressure: ");
  Serial.print(pressure / 100.0); // Convert pressure to hPa
  Serial.print(" hPa ");
  digitalWrite(dht_dpin1, LOW);
  digitalWrite(dht_dpin2, LOW);
    String message1 = "field1=" + String(temperature1);
    String message2 = "field2=" + String(humidity1);
    String message3 = "field3=" + String(temperature2);
    String message4 = "field4=" + String(humidity2);
    String message5 = "field5=" + String(pressure/100.0);
    String topicString = "channels/" + String(channelID) + "/publish";
    mqttClient.publish(topicString.c_str(), message1.c_str() );
    mqttClient.publish(topicString.c_str(), message2.c_str() );
    mqttClient.publish(topicString.c_str(), message3.c_str() );
    mqttClient.publish(topicString.c_str(), message4.c_str() );
    mqttClient.publish(topicString.c_str(), message5.c_str() );
   
  delay(15000);
  digitalWrite(dht_dpin1, HIGH);
  digitalWrite(dht_dpin2, HIGH);
}