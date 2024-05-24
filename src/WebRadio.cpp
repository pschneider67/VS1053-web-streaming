/**
 * @file    WebRadio.cpp
 * @author  Peter Schneider
 * @version 0.1
 * @date    02.05.2024
 * 
 * @copyright Copyright (c) 2024
 */

#include <vs1053_ext.h>
#include <WiFiManager.h>
#include <TFT_eSPI.h>
#include <Preferences.h>    // for reading and writing into the ROM memory

#include "WebRadio.h"

#include "psGpio.h"
#include "tft.h"

// LED output
clOut led1(LED_1, POLARITY::POS);
clOut led2(LED_2, POLARITY::POS);
clOut led3(LED_3, POLARITY::POS);

// Mute
clOut SpeakerOn(MUTE, POLARITY::POS);       // switch speaker on

// switches
clIn sw1(SW_1, 40, 2000, POLARITY::NEG);
clIn sw2(SW_2, 40, 2000, POLARITY::NEG);
clIn sw3(SW_3, 40, 2000, POLARITY::NEG);
clIn sw4(SW_4, 40, 2000, POLARITY::NEG);

// mp3 radio
#define ST_MAX      16               
#define MP3_VOL_MAX 50

const station_t StationList[ST_MAX] PROGMEM = {
  {1,  "HR-Info",             "https://dispatcher.rndfnk.com/hr/hrinfo/live/mp3/high"},                 
  {2,  "HR 1",                "https://dispatcher.rndfnk.com/hr/hr1/live/mp3/high"},  
  {3,  "HR 3",                "https://dispatcher.rndfnk.com/hr/hr3/mittelhessen/high"},                   
  {4,  "harmony.fm",          "http://mp3.harmonyfm.de/harmonyfm/hqlivestream.mp3"},                     
  {5,  "Antenne 1 80er",      "http://stream.antenne1.de/80er/livestream2.mp3"},                         
  {6,  "SWR 3",               "https://liveradio.swr.de/sw282p3/swr3/play.mp3"},                         
  {7,  "NDR 1",               "http://icecast.ndr.de/ndr/ndr1wellenord/kiel/mp3/128/stream.mp3"},        
  {8,  "Hit Radio FFH",       "http://mp3.ffh.de/radioffh/hqlivestream.mp3"},                            
  {9,  "Radio Arabella",      "https://live.radioarabella.de/stream"},
  {10,  "80er-Kulthits",       "http://mp3channels.webradio.antenne.de/80er-kulthits"},
  {11, "BR Klassik",          "http://streams.br.de/br-klassik_2.m3u"},
  {12, "Die neue Welle",      "http://www.meine-neue-welle.de/dnw_128.m3u"},
  {13, "Radio BoB AC/DC",     "http://streams.radiobob.de/bob-acdc/mp3-128/streams.radiobob.de/"},
  {14, "Rock Antenne",        "http://mp3channels.webradio.antenne.de/rockantenne"},
  {15, "BR Oldies",           "http://streams.br.de/bayern1_2.m3u"},
  {16, "BR Pop/Rock",         "http://streams.br.de/bayern3_2.m3u"}
}; 

VS1053      mp3(VS1053_CS, VS1053_DCS, VS1053_DREQ, VSPI, VS1053_MOSI, VS1053_MISO, VS1053_SCK);
cDisplay    tft(ROTATION_90, TFT_BLACK, TFT_WHITE);
WiFiManager wifiManager;
WiFiClient  client;
Preferences pref;

uint32_t u32DebugTimer = 0;

uint16_t u16Vol = 0;
uint16_t u16Station = 0;
uint8_t  u8Tone = 15;

String strLocalIp;

void setup() {
  Serial.begin(115200);

  Serial.println(F("---------------------------"));
	Serial.println(F("- ESP32 Web Radio VS1053b -"));
	Serial.println(F("---------------------------"));

  printCpuInfo();
    
  led1.On();
  initWifi();

  // init config 
  tft.showFrame();

  Serial.println(client.localIP());
  tft.showLowerSection(strLocalIp.c_str());

  pref.begin("WebRadio", false);
  u16Vol = pref.getShort("Vol", 2);
  tft.showVolume(u16Vol);
  u16Station = pref.getShort("Station", 2);
  tft.showStation(&StationList[u16Station]);
  
  // init VS1053 player
  mp3.begin();
  mp3.setVolumeSteps(MP3_VOL_MAX);
  mp3.setVolume(u16Vol);
  mp3.setTone(&u8Tone);
  mp3.connecttohost(StationList[u16Station].Url);  // start with last station

  tft.showStation(&StationList[u16Station]);

  Serial.println(F(".. WebRadio init done"));          // Debug  
              
  Serial.println( );
  led2.On();
}

void loop() { 
  mp3.loop();             // mp3 Steuerung und Wiedergabe
  
  gpioHandling();       

  // toggle LED 3 -- only for debugging
  if (millis() > u32DebugTimer + 1000) {
    led3.Toggle();
    u32DebugTimer = millis();
  }
}

// -----------------------------------------------------------------------------------
// serial print cpu info
// -----------------------------------------------------------------------------------
void printCpuInfo(void) {
  Serial.println();
  Serial.println(F("---------------"));
  Serial.println(F("- System Info -"));
  Serial.println(F("---------------"));
  
  Serial.print(String(F("esp32 cores   - ")));
	Serial.println(ESP.getChipCores());

	Serial.print(String(F("psram         - ")));
	Serial.print(ESP.getPsramSize() / 1024);
	Serial.println(String(F(" kB")));

	Serial.print(String(F("heap size     - ")));
	Serial.print(ESP.getHeapSize() / 1024);
	Serial.println(String(F(" kB")));

	Serial.print(String(F("sktech memory - ")));
	Serial.print(ESP.getFreeSketchSpace() / 1024);
	Serial.println(String(F(" kB")));
  Serial.println("");
}

// -----------------------------------------------------------------------------------
// gpio handling
// -----------------------------------------------------------------------------------
void gpioHandling(void) {
  sw1.runState();
	sw2.runState();
  sw3.runState();
	sw4.runState();

  changeVol();          
  changeStation();
}

// -----------------------------------------------------------------------------------
// vol+ --> switch 2 / vol- --> switch 1
// -----------------------------------------------------------------------------------
void changeVol(void) {
  static uint16_t u16State = 0;
  static uint16_t u16VolOld = 1;
 
  switch (u16State) {
    case 0:
      sw2.incValue(&u16Vol, MP3_VOL_MAX, 1, false);
      sw1.decValue(&u16Vol, MP3_VOL_MAX, 1, false);
      if (u16VolOld != u16Vol) {
        u16State = 10;
      }
      break;
    case 10:
      mp3.setVolume(u16Vol);
      pref.putShort("Vol", u16Vol);
      tft.showVolume(u16Vol);
      Serial.print(F("Vol     - "));
      Serial.println(u16Vol);
      u16VolOld = u16Vol;
      u16State = 0;
      break;
    default:
      u16State = 0;
      break;
  }
}

// -----------------------------------------------------------------------------------
// station+ --> switch 4 / station- --> switch 3
// -----------------------------------------------------------------------------------
void changeStation(void) {
  static uint16_t u16State = 0;
  static uint32_t u32TimeOut = 0;
  static uint16_t u16StationOld = 1;

  switch (u16State) {
    case 0:
      sw4.incValue(&u16Station, ST_MAX - 1, 1, true);
      sw3.decValue(&u16Station, ST_MAX - 1, 1, true);
      if (u16StationOld != u16Station) {
        SpeakerOn.Off();
        u32TimeOut = millis();   
        u16State = 10;
      }
      break;
    case 10:
      if (millis() > u32TimeOut + 100) {
        u16State = 20;
      }
      break;
    case 20:
      mp3.connecttohost(StationList[u16Station].Url);
      pref.putShort("Station", u16Station);
      tft.showStation(&StationList[u16Station]);
      Serial.print(F("Station - "));
      Serial.println(u16Station);
      u16StationOld = u16Station;
      u16State = 0;
      break;
    default:
      u16State = 0;
      break;
  }
}

// ---------------------------------------------------------------------------------------------------
// init wifi 
// ---------------------------------------------------------------------------------------------------
void  initWifi(void) {
  wifiManager.setAPCallback(wifiCallback);
	wifiManager.setSaveConfigCallback(saveConfigCallback);
    
  tft.setFreeFont(DEFAULT_FONT);
  tft.setTextSize(1);	
  tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 30);
  
  tft.drawString(F(".. start wifi"), 10, 10);
  Serial.println(F(".. start wifi"));  // Debug
  
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); 
  WiFi.begin();
  
  if (wifiManager.autoConnect("ESP32_WEB_RADIO")) {
  	tft.drawString(F(".. wifi connected"), 10, 40);
		strLocalIp = WiFi.SSID() + String(" - ") + WiFi.localIP().toString();
		String strText = String(".. ") + WiFi.SSID() + String(" - ") + WiFi.localIP().toString();
		tft.drawString(strText, 10, 70);
    delay(2000);
  } else {
		tft.drawString(F(".. WLan error !!"), 10, 40);
		tft.drawString(F(".. ESP Reset !!"), 10, 70);
		delay(2000);
		while (true) {;}
	}
}

// wifi callbacks
void wifiCallback(WiFiManager *_myWiFiManager) {
	tft.fillScreen(TFT_BLACK);
	tft.drawString(F(".. enter wifi config mode"), 10, 10);
  String strText = String(".. ") + _myWiFiManager->getConfigPortalSSID();
  tft.drawString(strText, 10, 30);
  tft.drawString(F("-- IP 192.168.4.1"), 10, 70);
}

void saveConfigCallback(void) {
	Serial.println(F(".. wifi data saved"));
}

// ---------------------------------------------------------------------------------------------------
// optional infos
// ---------------------------------------------------------------------------------------------------
void vs1053_info(const char *info) {                // called from vs1053
    //Serial.print("DEBUG:        ");
    Serial.println(info);                           // debug infos
}

void vs1053_showstreamtitle(const char *info) {     // called from vs1053
    //Serial.print("STREAMTITLE:  ");
    //Serial.println(info);                         // Show title
    SpeakerOn.On();
    tft.showSongTitle(info);
}

/*
void vs1053_showstation(const char *info) {         // called from vs1053
    Serial.print("STATION:      ");
    Serial.println(info);                           // Show station name
    showStation(info);
}

void vs1053_bitrate(const char *br) {               // called from vs1053
    Serial.print("BITRATE:      ");
    Serial.println(String(br)+"kBit/s");            // bitrate of current stream
}

void vs1053_showstreaminfo(const char *info) {      // called from vs1053
    Serial.print("STREAMINFO:   ");
    Serial.println(info);                           // Show streaminfo
}

void vs1053_eof_mp3(const char *info) {             // called from vs1053
    Serial.print("vs1053_eof:   ");
    Serial.print(info);                             // end of mp3 file (filename)
}
void vs1053_commercial(const char *info) {          // called from vs1053
    Serial.print("ADVERTISING:  ");
    Serial.println(String(info)+"sec");             // info is the duration of advertising
}
void vs1053_icyurl(const char *info) {              // called from vs1053
    Serial.print("Homepage:     ");
    Serial.println(info);                           // info contains the URL
}
void vs1053_eof_speech(const char *info) {          // called from vs1053
    Serial.print("end of speech:");
    Serial.println(info);
}
void vs1053_lasthost(const char *info) {            // really connected URL
    Serial.print("lastURL:      ");
    Serial.println(info);
}
*/