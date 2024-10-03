/**
 * @file    WebRadio.cpp
 * @author  Peter Schneider
 * @version 0.1
 * @date    02.05.2024
 * 
 * @copyright Copyright (c) 2024
 */

// teste auch diese Version
// https://github.com/CelliesProjects/ESP32_VS1053_Stream/blob/master/examples/simple/simple.ino
// https://duino4projects.com/hifi-online-radio-internet-streaming-with-esp32-and-vs1053/

#include <Arduino.h>
#include <vs1053_ext.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include <Preferences.h>    // for reading and writing into the ROM memory

#include "WebRadio.h"
#include "psGpio.h"
#include "psTft.h"
#include "psServer.h"

extern WebServer server;

// LED output
clOut led1(LED_1, POLARITY::POS);
clOut led2(LED_2, POLARITY::POS);
clOut led3(LED_3, POLARITY::POS);

// Mute
clOut SpeakerOn(MUTE, POLARITY::POS);       // switch speaker on

// switches
clIn sw1(SW_1, 40, 1000, POLARITY::NEG);    // volume -
clIn sw2(SW_2, 40, 1000, POLARITY::NEG);    // volume +
clIn sw3(SW_3, 40, 1000, POLARITY::NEG);    // station -
clIn sw4(SW_4, 40, 1000, POLARITY::NEG);    // station +

station_t StationList[ST_MAX];
station_t StationListDefault[ST_MAX] = {
  {1,  "HR-Info",             "https://dispatcher.rndfnk.com/hr/hrinfo/live/mp3/high"},                 
  {2,  "HR 1",                "https://dispatcher.rndfnk.com/hr/hr1/live/mp3/high"},  
  {3,  "HR 3",                "https://dispatcher.rndfnk.com/hr/hr3/mittelhessen/high"},                   
  {4,  "harmony.fm",          "http://mp3.harmonyfm.de/harmonyfm/hqlivestream.mp3"},                     
  {5,  "Antenne 1 80er",      "http://stream.antenne1.de/80er/livestream2.mp3"},                         
  {6,  "SWR 3",               "https://liveradio.swr.de/sw282p3/swr3/play.mp3"},                         
  {7,  "NDR 1",               "http://icecast.ndr.de/ndr/ndr1wellenord/kiel/mp3/128/stream.mp3"},        
  {8,  "Hit Radio FFH",       "http://mp3.ffh.de/radioffh/hqlivestream.mp3"},                            
  {9,  "Radio Arabella",      "https://live.radioarabella.de/stream"},
  {10, "80er-Kulthits",       "http://mp3channels.webradio.antenne.de/80er-kulthits"},
  {11, "Die neue Welle",      "http://www.meine-neue-welle.de/dnw_128.m3u"},
  {12, "Radio BoB AC/DC",     "http://streams.radiobob.de/bob-acdc/mp3-128/streams.radiobob.de/"},
  {13, "Rock Antenne",        "http://mp3channels.webradio.antenne.de/rockantenne"},
  {14, "BR Oldies",           "http://streams.br.de/bayern1_2.m3u"},
  {15, "BR Pop/Rock",         "http://streams.br.de/bayern3_2.m3u"},
  {16, "BR Klassik",          "http://streams.br.de/br-klassik_2.m3u"}
}; 

VS1053        mp3(VS1053_CS, VS1053_DCS, VS1053_DREQ, VSPI, VS1053_MOSI, VS1053_MISO, VS1053_SCK);
cDisplay      tft(ROTATION_90, TFT_BLACK, TFT_WHITE);
WiFiManager   wifiManager;
WiFiClient    client;
Preferences   pref;
WebServer     server(80);

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
  serverInit();
 
  // init config 
  tft.showFrame();

  Serial.println(client.localIP());
  tft.showStatusLine(strLocalIp.c_str());

  pref.begin("WebRadio", false);
  u16Station = pref.getShort("Station", 0);
  u16Vol     = pref.getShort("Vol", 0);
 
  resetStationData();
  getStationList(StationList);
  
  tft.showVolume(u16Vol);
  tft.showStation(&StationList[u16Station]);
  
  // init VS1053 player
  mp3.begin();
  mp3.setVolumeSteps(MP3_VOL_MAX);
  mp3.setVolume(u16Vol);
  mp3.setTone(&u8Tone);
  mp3.connecttohost(StationList[u16Station].Url);     // start with last station

  tft.showStation(&StationList[u16Station]);

  Serial.println(F(".. WebRadio init done"));         // Debug  
              
  Serial.println( );
  led2.On();
}

void loop() { 
  mp3.loop();             // mp3 control an play
  gpioHandling();       
  tft.scrollText();
  server.handleClient();
}   

void getStationList(station_t* pStationList) {
  for (int i = 0; i < ST_MAX; i++) {
    String key = "strStation" + String(i); 
    String strDummy = pref.getString(key.c_str(), "unknown name");  
    strDummy.toCharArray(pStationList[i].Name, 40); 

    key = "strUrl" + String(i); 
    strDummy = pref.getString(key.c_str(), "unknown url");  
    strDummy.toCharArray(pStationList[i].Url, 100); 
  }
}

void setStationList(station_t* pStationList) {
  for (int i = 0; i < ST_MAX; i++) {
    String key = "strStation" + String(i);  
    pref.putString(key.c_str(), pStationList[i].Name);  
    Serial.println("Gespeichert: " + String(pStationList[i].Name) + " mit key: " + key);

    key = "strUrl" + String(i);  
    pref.putString(key.c_str(), pStationList[i].Url);  
    Serial.println("Gespeichert: " + String(pStationList[i].Url) + " mit key: " + key);
  }
 
  tft.showStation(&StationList[u16Station]);
}

void resetStationData(void) {
  for (int i = 0; i < ST_MAX; i++) {
    StationList[i] = StationListDefault[i];
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
  static uint16_t u16State = 0;
  static uint32_t u32Timer;

  sw1.runState();
  sw4.runState();

  switch (u16State) {
    case 0:
      u32Timer = millis();
      u16State = 10;  
      break;
    case 10:
      if (millis() - u32Timer >= 10) {
        sw2.incValue(&u16Vol, MP3_VOL_MAX, 1, false, true);
        sw1.decValue(&u16Vol, MP3_VOL_MAX, 1, false, true);
        u32Timer = millis();
        u16State = 20;
      }
      break;
    case 20:
      if (millis() - u32Timer >= 10) {
        sw4.incValue(&u16Station, ST_MAX - 1, 1, true, false);
        sw3.decValue(&u16Station, ST_MAX - 1, 1, true, false);
        u32Timer = millis();
        u16State = 30;  
      }
      break;
    case 30:
      if (millis() - u32Timer >= 10) {
        // reset all station data
        if (sw1.Status() & sw4.Status()) {
          Serial.println("-- reset station data");
          resetStationData();
          setStationList(StationListDefault);
          u16State = 40;
        } else {
          changeVol();          
          changeStation();
          u16State = 0;
        }
      }
      break;
    case 40:
      if (!sw1.Status() & !sw4.Status()) {
        u16State = 0;   
      } 
      break;
    default:
      u16State = 0;
      break;
  }

  // toggle LED 3 -- only for debugging
  if (millis() - u32DebugTimer >= 1000) {
    led3.Toggle();
    u32DebugTimer = millis();
  }
}

// -----------------------------------------------------------------------------------
// vol+ --> switch 2 / vol- --> switch 1
// -----------------------------------------------------------------------------------
void changeVol(void) {
  static uint16_t u16State = 0;
  static uint16_t u16VolOld = 1;
  static uint32_t u32TimeOut = 0;

  switch (u16State) {
    case 0:
      if (u16VolOld != u16Vol) {
        mp3.setVolume(u16Vol);
        pref.putShort("Vol", u16Vol);
        tft.showVolume(u16Vol);
        Serial.println(String(F("Vol - ")) + String(u16Vol));
        u16VolOld = u16Vol;
        u32TimeOut = millis();
        u16State = 10;
      }
      break;
    case 10:
      if (millis() - u32TimeOut >= 20) {
        u16State = 0;
      }
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
      if (u16StationOld != u16Station) {
        SpeakerOn.Off();
        mp3.connecttohost(StationList[u16Station].Url);
        pref.putShort("Station", u16Station);
        tft.showStation(&StationList[u16Station]);
        Serial.println(String(F("Station - ")) + String(u16Station));
        u16StationOld = u16Station;
        u32TimeOut = millis();  
        u16State = 10;
      }
      break;
    case 10:
      if (millis() - u32TimeOut >= 20) {
        u16State = 0;
      }
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
  tft.drawString(String(".. ") + _myWiFiManager->getConfigPortalSSID(), 10, 30);
  tft.drawString(F("-- IP 192.168.4.1"), 10, 70);
}

void saveConfigCallback(void) {
	Serial.println(F(".. wifi data saved"));
}

// ---------------------------------------------------------------------------------------------------
// optional infos
// ---------------------------------------------------------------------------------------------------
void vs1053_info(const char *info) {                // called from vs1053
    Serial.print("DEBUG:        ");
    Serial.println(info);                           // debug infos
}

void vs1053_showstreamtitle(const char *info) {     // called from vs1053
    //Serial.print("STREAMTITLE:  ");
    //Serial.println(info);                         // Show title
    SpeakerOn.On();
    tft.showSongTitle(info);
}

void vs1053_bitrate(const char *br) {               // called from vs1053
    String strData = String("Bitrate: ") + String((String(br).toInt() / 1000)) + String("kBit/s");
    Serial.println(strData);                        // bitrate of current stream
    tft.showBitrate(strData);
}

void vs1053_showstation(const char *info) {         // called from vs1053
    Serial.print("STATION:      ");
    Serial.println(info);                           // Show station name
}

/*
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