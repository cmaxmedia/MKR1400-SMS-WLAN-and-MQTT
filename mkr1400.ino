#include <MKRGSM.h>
#define PINNUMBER "0000" //your GSM pin here

GSM gsmAccess;     // include a 'true' parameter for debug enabled
GSM_SMS sms;
GSMVoiceCall vcs;

boolean IsGSMConnected = false;

GSMClient net;

void setup(void) 
{
  Serial1.begin( 9600 );
  Serial.begin( 9600 );
  Serial.println( "Ready" );
  
  pinMode(LED_BUILTIN, OUTPUT);  

  blinkit();
  blinkit();
  blinkit();
  
  ConnectGSM();
  Serial.println( "GSM: Done" );

  vcs.hangCall();
  Serial.println( "Waiting for SMS or Call" );
  

  blinkit();
  blinkit();
  blinkit();
  
  
}

void loop(void) {
  char remoteNumber[20];  // Holds the emitting number
  String writeString = "";
  
  //Serial1 Event from esp8266
  String serialData2 = readSerial1();
  if( serialData2!="" ) {
    Serial.println( "rcv Start" );
      
    if( serialData2.startsWith( "PrepareSMS" ) ) {
      
      //make valid sms text
      String sendData = serialData2;
      sendData.replace( "\r", "" );
      sendData.replace( "\n", "" );


      //split parts coming from esp8266
      String part0 = getValue( sendData, "_", 0 ); //PrepareSMS
      String part1 = getValue( sendData, "_", 1 ); //Text
      String part2 = getValue( sendData, "_", 2 ); //Number

      //first part is the identification which can be ignored here
      //second part is the sms text
      //third part is the number
      char smsNum[20];
      part2.toCharArray( smsNum, 20 );

      Serial.println( "Text" );
      Serial.println( part1 );
      Serial.println( "Num" );
      Serial.println( smsNum );

      
      if( part2!="" ) {
        blinkfast();
        blinkfast();
        blinkfast();

        Serial.println( "Send" );
        SendSMS( smsNum, part2 );

        delay( 1000 );

        blinkfast();
        blinkfast();
        blinkfast();

        Serial.println( "Send done" );

      }
  
    }

    Serial.println( "rcv Done" );
    serialData2 = "";
    delay( 2000 );
    
  }
    

  //incoming sms
  if( sms.available() ) {

    blinkit();
    blinkit();
    blinkit();


    Serial.println( "Rcv SMS" );
    
    int c;
    String smsText;
    
    //get number and read sms text
    sms.remoteNumber(remoteNumber, 20);

    while((c=sms.read())!=-1 ) {
      smsText +=  (char) c;  
    }



    blinkfast();
    blinkfast();
    blinkfast();
    delay( 1000 );

    Serial.println( "Reading SMS" );
    
    //build string
    writeString = "SMSReceived_";
    writeString += smsText;
    writeString += "_";
    writeString += remoteNumber;

    //send String back to ESP8266
    Serial1.println( writeString );

    //delete sms
    sms.flush();

    Serial.println( "sms don" );

    blinkfast();
    blinkfast();
    blinkfast();
    delay( 1000 );
    
    
  }


  //call event
  switch (vcs.getvoiceCallStatus()) 
  {
    case IDLE_CALL: // Nothing is happening
      break;
    
    case CALLING: // This should never happen, as we are not placing a call
      Serial.println("CALLING");
      break;
    
    case RECEIVINGCALL: // Yes! Someone is calling us
      Serial.println("RECEIVING CALL");
      // Retrieve the calling number
      vcs.retrieveCallingNumber(remoteNumber, 20);
      
      // Print the calling number
      Serial.print("Number:");
      Serial.println(remoteNumber);
      
      //hang up an prepare info sms for mqtt
      vcs.hangCall();

      //build payload
      writeString = "SMSReceived_";
      writeString += "Incoming Call";
      writeString += "_";
      writeString += remoteNumber;
  
      //send payload back to ESP8266
      Serial1.println( writeString );
  
      Serial.println( "call don" );
  
      blinkfast();
      blinkfast();
      blinkfast();
      delay( 1000 );

      break;
    
    case TALKING:  // In this case the call would be established
    
      Serial.println("TALKING. Enter line to interrupt.");
      while(Serial.read()!='\n')
      delay(100);
      vcs.hangCall();
      Serial.println("HANG. Waiting Call.");      
      break;
  }
}

void SendSMS( char* smsNum, String tx ) {

  sms.beginSMS( smsNum );
  sms.print(tx);
  sms.endSMS(); 


}

void ConnectGSM() {
  
  boolean connected = false;
    
   while(!connected) {
     if(gsmAccess.begin(PINNUMBER)==GSM_READY){
       
       Serial.println("Connected to network");
       IsGSMConnected=true;
       connected = true;
       
     }
     else
     {
       Serial.println("Not connected");
       IsGSMConnected=false;
       
     }
   }
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
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}

void blinkfast() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(150);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(150);                       // wait for a second
}
