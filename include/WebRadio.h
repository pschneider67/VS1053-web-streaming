/*
 * WebRadio.h
 *
 *  Created on: 02.05.2024
 *      Author: Peter Schneider
 */

#pragma once

#include "Arduino.h"
#include "WiFiManager.h"

#define UNDEFINED  -1

// mp3 radio
#define ST_MAX      16               
#define MP3_VOL_MAX 50
typedef struct {
  uint16_t Number;
  char     Name[40]; 
  char     Url[100];
} station_t;

// define LED's
#define LED_1   22
#define LED_2   21
#define LED_3   25

#define MUTE    2

// define Switches
#define SW_1    27
#define SW_2    39
#define SW_3    35
#define SW_4    34

// VS1053
#define VS1053_CS     5
#define VS1053_DCS    16
#define VS1053_DREQ   4

#define VS1053_MOSI   23
#define VS1053_MISO   19
#define VS1053_SCK    18

void initWifi(void);
void wifiCallback(WiFiManager*);
void saveConfigCallback(void);

void printCpuInfo(void);
void gpioHandling(void);
void changeVol(void);
void changeStation(void);
void getStationList(station_t*);
void setStationList(station_t*);

// data from VS1053
void vs1053_info(const char *info);                 
void vs1053_showstation(const char *info);          
void vs1053_showstreamtitle(const char *info);      
void vs1053_showstreaminfo(const char *info);       
void vs1053_eof_mp3(const char *info);              
void vs1053_bitrate(const char *br);                
void vs1053_commercial(const char *info);           
void vs1053_icyurl(const char *info);               
void vs1053_eof_speech(const char *info);           
void vs1053_lasthost(const char *info);             // really connected URL
