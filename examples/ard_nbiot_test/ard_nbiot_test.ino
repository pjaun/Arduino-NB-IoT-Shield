/*
 * ard_nbiot_test.c
 * Sixfab NB-IoT Shield Test Code 
 * 
 * Created by Yasin Kaya (selengalp)
 * 17.01.2017
 * yasinkaya.121@gmail.com
 */

#include "ard_nbiot_test.h"

// ------------------------------------------------------------------
// ------------------------- SETUP ----------------------------------
// ------------------------------------------------------------------
void setup() {
  // start bc95 uart connection
  // baudrate 9600
  bc95.begin(9600);
  delay(200);

  // start debug soft uart connection
  // baudrate 9600
  debug.begin(9600);
  delay(200);
  debug.print("\nArduino NB-IOT Shield Test Code\n");
  debug.print("\nStarted\n");

  pinMode(USER_BUTTON, INPUT);
  pinMode(USER_LED, OUTPUT);
  pinMode(BC95_ENABLE, OUTPUT);
  pinMode(ALS_PT19_PIN,INPUT);
  pinMode(RELAY,OUTPUT);

  digitalWrite(BC95_ENABLE, HIGH);
  debug.print("\nBC95 Enabled\n");

  delay(1000);

  // HDC1080 begin
  hdc1080.begin(0x40);

  // mma8452q init 
  accel.init();
  
  // User LED On
  digitalWrite(USER_LED, LOW);

  // initialize server connection
  initBC95();

  // sending test data
  sendDataUDP("sixfab",TIMEOUT);
 // end of setup 
}

// ------------------------------------------------------------------
// ------------------------- LOOP -----------------------------------
// ------------------------------------------------------------------
void loop() {
  
#ifdef RELAY_TEST
// relay
  digitalWrite(5, HIGH);
  delay(10000);
  digitalWrite(5, LOW);
  delay(10000);
#endif 

#ifdef ECHO_TEST // command echo test
  // if any command, deliver the bc95 serial   
  while(debug.available()){
    bc95.write(debug.read());
  }

// if any response from bc95, deliver the debug serial
  while(bc95.available()){
    debug.write(bc95.read());
  }
#endif 

#ifdef ACCEL_TEST
  //Accelerometer
  if (accel.available())
  {
    accel.read();
    uint8_t pl = accel.readPL();

    switch (pl)
  {
  case PORTRAIT_U:
    sprintf(data_compose,"%s","Portrait Up");
    break;
  case PORTRAIT_D:
    sprintf(data_compose,"%s","Portrait Down");
    break;
  case LANDSCAPE_R:
    sprintf(data_compose,"%s","Landscape Right");
    break;
  case LANDSCAPE_L:
  sprintf(data_compose,"%s","Landscape Left");
    break;
  case LOCKOUT:
    sprintf(data_compose,"%s","Flat");
    break;
  }
    //printCalculatedAccels();
    //delay(20);
    //debug.print(data_compose);
    sendDataUDP(data_compose,5000);
  }
#endif

#ifdef SENSOR_TEST
    lux = analogRead(ALS_PT19_PIN);
    temperature = hdc1080.readTemperature();
    humidity = hdc1080.readHumidity();
    sprintf(data_compose,"%s%d-%s%d-%s%d","Lux:",lux,"Temp:",temperature, "Hum:",humidity);
    delay(100);
    //debug.println(data_compose);
    sendDataUDP(data_compose,5000);
#endif 

delay(5000);
// end of loop   
}

// ------------------------------------------------------------------
// ---------------------- FUNCTIONS ---------------------------------
// ------------------------------------------------------------------

// send at command. read the response. if response is desired one, OK. 
// else repeat AT command until get desired response. 
void send_at_command(const char * command, const char *desired_reponse, uint16_t timeout){
  uint32_t timer;

  memset(response, 0 , 400);
  bc95.flush();
  delay(2000);
  bc95.print(command);
  bc95.print("\r");
 
  timer = millis();
  while(true){
    if(millis()-timer > timeout){
      bc95.print(command);
      bc95.print("\r");

      debug.print(response);
      timer = millis();
    }
    char c;
    int i = 0; 

    while(bc95.available()){
      c = bc95.read();
      debug.write(c);
      response[i++]=c;
      delay(2);
      }
      if(strstr(response, desired_reponse)){
      memset(response, 0 , strlen(response));
      break;
      }    
  }
}

void sendDataUDP(char *data, uint16_t timeout){
  char compose[200]="";
  char data_hex[200];
  char data_len[5];
  
  for(int i= 0; i<strlen(data); i++){
    sprintf(data_hex+i*2, "%02X", data[i]);
    }  

  sprintf(data_len, "%d", strlen(data));

  strcat(compose, "AT+NSOST=0,");
  strcat(compose, ip);
  strcat(compose, ",");
  strcat(compose, port);
  strcat(compose, ",");
  strcat(compose, data_len);
  strcat(compose, ",");
  strcat(compose, data_hex);

  debug.print("\n"); 
  debug.print(compose);
  debug.print("\n");

  send_at_command(compose,"OK\r\n",timeout);
  }
  
void initBC95(){
  send_at_command("AT","OK\r\n",TIMEOUT);  
  send_at_command("ATE1","OK\r\n",TIMEOUT);
  send_at_command("AT","OK\r\n",TIMEOUT);
  send_at_command("AT+CGATT=1","OK\r\n",TIMEOUT);
  send_at_command("AT+CGATT?","+CGATT:1\r\n",TIMEOUT);
  send_at_command("AT+CSQ","OK\r\n",TIMEOUT);
  send_at_command("AT+CGDCONT=1,\"IP\",\"INTERNET\"","OK\r\n",TIMEOUT);
  send_at_command("AT+NSOCR=DGRAM,17,3005,0","OK\r\n",TIMEOUT);
  send_at_command("AT+NSOST=0,78.173.113.44,4000,5,68656c6c6f","OK\r\n",TIMEOUT);
}

void printCalculatedAccels()
{ 
  debug.print(accel.cx, 3);
  debug.print("    ");
  debug.print(accel.cy, 3);
  debug.print("    ");
  debug.print(accel.cz, 3);
  debug.print("    ");
  debug.print("\n");
}
    


