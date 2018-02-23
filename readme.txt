Connect your Arduino MKR 1400 via Serial with a NodeMCU ESP8266 over WiFi with an local MQTT Broker

MKR1400 RX/TX -> ESP8266 TX/RX

MQTT -> ESP8266 -> MKR1400
 |- MQTT: Send Topic "CMAXMEDIA/SMS" with Payload "PrepareSMS_This is a SMS_+436761234567"
 |- ESP8266: MQTT On Message
 |- ESP8266: Serial.Println to connected MKR1400

 |- MKR1400: Listen to Serial1 coming from ESP8266
 |- MKR1400: Message Arrived, then split and Send SMS to according Parameters

MKR1400 Call or SMS -> ESP8266 -> MQTT
 |- MKR1400: Incoming Call or SMS
 |- MKR1400: Prepare String "SMSReceived_This is the Message_+436761234567"
 |- MKR1400: Send String over to ESP8266 using Serial1
 |- ESP8266: Listen to Serial to String that starts with "SMSReceived"
 |- ESP8266: Split String and concate part 2 and 3 as payload
 |- ESP8266: MQTT Publish with payload to "CMAXMEDIA/SMS/RECEIVED"
