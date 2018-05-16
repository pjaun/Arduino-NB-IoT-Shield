/*
 * gettingStarted.ino
 * Sixfab NB-IoT Shield Test Code 
 * 
 * Created by Yasin Kaya (selengalp)
 * 17.01.2017
 * yasinkaya.121@gmail.com
 * 
 * Modified by saeedjohar
 */
#include <SoftwareSerial.h>
#define bc95 Serial
#define BC95_ENABLE 4
#define TIMEOUT 1000

//Global Variables
char response[400];
// ########################################

// UDP Server Configuration
// Change with your server information
// ########################################
char ip[15] = "XX.XX.XX.XX"; //Local IP of your router
char port[10] = "XXXX"; //Service Port

SoftwareSerial debug(10, 11); //(RX, TX)

//Functions Declaration
void initBC95();
void send_at_command(const String, const char *, uint16_t); 
void sendDataUDP(char *, uint16_t);

// ------------------------------------------------------------------
// ------------------------- SETUP ----------------------------------
// ------------------------------------------------------------------
void setup() {
  // Start BC95 UART Connection with 9600 baudrate
  bc95.begin(9600);
  delay(200);
  // Start debug soft UART Connection with 96200 baudrate
  debug.begin(9600);
  delay(200);
  
  pinMode(BC95_ENABLE, OUTPUT);
  digitalWrite(BC95_ENABLE, HIGH);
  debug.print("\nBC95 Enabled\n");
  delay(1000);
  initBC95(); //initialize server connection
}
// ------------------------------------------------------------------
// --------------------------- LOOP ---------------------------------
// ------------------------------------------------------------------
void loop() {
  sendDataUDP(" SIXFAB ",TIMEOUT);
  delay(5000);

}

// ------------------------------------------------------------------
// ----------------- FUNCTION DEFINITION ----------------------------
// ------------------------------------------------------------------

void initBC95(){
  send_at_command("ATE1","OK\r\n",TIMEOUT);
  send_at_command("AT","OK\r\n",TIMEOUT);  
  send_at_command("ATE1","OK\r\n",TIMEOUT);
  send_at_command("AT+CGATT=1","OK\r\n",TIMEOUT);
  send_at_command("AT+CGATT?","+CGATT:1\r\n",TIMEOUT);
  send_at_command("AT+CSQ","OK\r\n",TIMEOUT);
  send_at_command("AT+CGDCONT=1,\"IP\",\"INTERNET\"","OK\r\n",TIMEOUT);
  send_at_command("AT+NSOCR=DGRAM,17,3005,0","OK\r\n",TIMEOUT);
  }//=====END OF initBC95====// 


void send_at_command(const char * command, const char *desired_response, uint16_t timeout){
  
  uint32_t timer;
  memset(response,0,400);
  bc95.flush();
  delay(2000);
  bc95.print(command);
  bc95.print("\r");

  timer = millis();
  while(true){
    if (millis()-timer > timeout){
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
      response[i++] = c;
      delay(2);
      }
     if(strstr(response, desired_response)){
      memset(response, 0, strlen(response));
      break;
      }   
    }
  }//=================END OF send_at_command==================//

void sendDataUDP(char *data, uint16_t timeout){
  char compose[200] = "";
  char data_hex[200];
  char data_len[5];

  for (int i = 0; i<strlen(data); i++){
    sprintf(data_hex+i*2, "%02X", data[i] );
    }//end of for
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
  }//==========END OF sendDataUDP===============//

