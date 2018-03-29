/*
 * ard_nbiot_test.h
 * Sixfab NB-IoT Shield Test Code 
 * 
 * Created by Yasin Kaya (selengalp)
 * 17.01.2017
 * yasinkaya.121@gmail.com
 */
#include <stdio.h>
#include <string.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Sixfab_HDC1080.h>
#include <Sixfab_MMA8452Q.h>
 
#define bc95 Serial
#define USER_BUTTON 8
#define USER_LED 6
#define BC95_ENABLE 4
#define ALS_PT19_PIN A1
#define RELAY 5
#define TIMEOUT 1000

// ########################################
// Activate desired tests

//#define ECHO_TEST
#define SENSOR_TEST
//#define RELAY_TEST
//#define ACCEL_TEST
// ########################################

// UDP Server Configuration
// Change with your server information
// ########################################
char ip[50] = "XX.XX.XX.XX"; // Local IP
char port[10] = "XXXX";  // Port
// ########################################

// DEBUGGING
// use ttl2usb converter cable to debug.
// ********* Debug Pinout Note ***********
// TTL2USB CABLE <------------>   ARDUINO
//       *                           *
//     BLACK     <------------>     GND
//     WHITE     <------------>   SOFT TX
//     GREEN     <------------>   SOFT RX
//     PURPLE    <------------>     NC 
SoftwareSerial debug(10, 11); // SOFT RX, SOFT TX

Sixfab_HDC1080 hdc1080;
MMA8452Q accel;

char response[400];
char data_compose[200] = "";
int lux;
int temperature;
int humidity;

void send_at_command(const String, const char *, uint16_t); 
void initBC95();
void sendDataUDP(char *, uint16_t);
void printCalculatedAccels();
