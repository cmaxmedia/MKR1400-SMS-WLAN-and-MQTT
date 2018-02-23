#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>

//MQTT Settings
AsyncMqttClient mqttClient;
#define MQTT_RELAY1_TOPIC     "CMAXMEDIA/SMS"
#define MQTT_RELAY1_TOPIC_1   "CMAXMEDIA/SMS/RECEIVED"
#define MQTT_LASTWILL_TOPIC   "CMAXMEDIA/LIGHT/lastwill"
#define MQTT_DEVICE_ID "SMSTerminal"


void setup() {
  // put your setup code here, to run once:

  Serial.begin( 9600 );
  blinkit();
  pinMode( LED_BUILTIN, OUTPUT );

  //Setup WiFi
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(MySSID , MyPSK );

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  blinkit();
  blinkit();
  
  //Setup MQTT - Thnx to Spiess A.
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(IPAddress(192, 168, 0, 75), 1883);
  mqttClient.setKeepAlive(5).setCleanSession(false).setWill(MQTT_LASTWILL_TOPIC, 2, true, "my wife will get all my money").setCredentials(MyMQTTUser, MyMQTTPass).setClientId( MQTT_DEVICE_ID );
  mqttClient.connect();

  blinkit();
  blinkit();

}

void loop() {

  String serialData = readSerial();
  if( serialData!="" ) {
    
    //incoming SMS
    if( serialData.startsWith( "SMSReceived" ) ) {

      blinkfast();
      blinkfast();
      blinkfast();

      //make payload
      String part1 = getValue( serialData, "_", 1 );
      String part2 = getValue( serialData, "_", 2 );
      String part3 = part1 + " von " + part2;
      
      
      char test[100];
      part3.toCharArray( test, 100 );
      
      //publish payload over MQTT
      mqttClient.publish(MQTT_RELAY1_TOPIC_1, 1, false, test ); 
      
    }

  }
  
}


void onMqttConnect(bool sessionPresent) {
  mqttClient.subscribe(MQTT_RELAY1_TOPIC, 1);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  mqttClient.connect();
  
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {}

void onMqttUnsubscribe(uint16_t packetId) {}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  //incoming topic
  if(strcmp( topic, MQTT_RELAY1_TOPIC )==0 )  {
     String writeString = "PrepareSMS_";
     writeString += payload;
     
     //write data to MKR1400
     Serial.println( writeString );
     delay( 1000 );
     
  }
}

void onMqttPublish(uint16_t packetId) {
}


//shared functions
String getValue(String data, String separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if ( (String) data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


String readSerial() {
  char rc;
  String receivedString;
  
  while (Serial.available() > 0) {
    delay( 3 );
    rc = Serial.read();
    receivedString += rc;
  
  }
  
  return receivedString;
    
}


String readSerial1() {
  char rc;
  String receivedString;
  
  while (Serial1.available() > 0) {
    delay( 3 );
    rc = Serial1.read();
    receivedString += rc;
  
  }
  
  return receivedString;
    
}

void blinkit() {
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}

void blinkfast() {
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(150);                       // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage LOW
  delay(150);                       // wait for a second
}
