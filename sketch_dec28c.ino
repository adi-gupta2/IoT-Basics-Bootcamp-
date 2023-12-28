const int trigpin = D0; //D4
const int echopin = D1;//D3
// long long int time = 0;
int distance = 0;

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>


const char *ssid ="AndroidAP";
const char *pass ="hellosista";
#define channelID 2389125
const char mqttUserName[] ="FRc9KQYfIBURETYRFysICjA";
const char clientID[] = "FRc9KQYfIBURETYRFysICjA";
const char mqttPass[] = "9zwmu9hvgApm+Uvnk5bWxUBp";
#define ESP8266wifi

const char* PROGMEM thingspeak_cert_thumbprint =
"271892dda426c30709b97ae6c521b95b48f716e1";

#define mqttPort 1883
WiFiClient client;

const char* server = "mqtt3.thingspeak.com";
int status = WL_IDLE_STATUS;
long lastPublishMillis = 0;
int connectionDelay = 1;
int updateInterval = 15;
PubSubClient mqttClient( client );

void mqttSubscribe( long subChannelID ){
String myTopic = "channels/"+String( subChannelID )+"/subscribe";
mqttClient.subscribe(myTopic.c_str());
}

void mqttPublish(long pubChannelID, String message) {
String topicString ="channels/" + String( pubChannelID ) + "/publish";
mqttClient.publish( topicString.c_str(), message.c_str() );
}

void mqttSubscriptionCallback( char* topic, byte* payload, unsigned int
length ) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();
}

void connectWifi() {
WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());
}

void mqttConnect() {
while ( !mqttClient.connected() )
{
if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) ) {
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
void setup() {
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
  Serial.begin(9600);
  // put your setup code here, to run once:
  connectWifi();
  mqttClient.setServer( server, mqttPort );
  mqttClient.setCallback( mqttSubscriptionCallback );
  mqttClient.setBufferSize( 2048 );

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
connectWifi();
}

if (!mqttClient.connected()) {
mqttConnect();
mqttSubscribe(channelID );
}

mqttClient.loop();
if ( (millis() - lastPublishMillis) > updateInterval*1000) {

  // put your main code here, to run repeatedly:
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin,LOW);
//
  float time  = pulseIn(echopin,HIGH);
  Serial.println(time);
  long dist = (time * 0.0034) / 2;
  delay(200);
  Serial.println(dist);
  
mqttPublish( channelID, (String("dist=")+String(dist)) );
lastPublishMillis = millis();

} 
}
