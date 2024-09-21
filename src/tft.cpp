/*
 * tft.cpp
 *
 *  Created on: 02.05.2024
 *      Author: Peter Schneider
 */
#include <string.h>

#include "tft.h"

cDisplay::cDisplay(uint8_t _rotation, uint32_t _backGroundColor, uint32_t _textColor)
{ 
  u8Rotation         = _rotation;
  u32BackGroundColor = _backGroundColor;
  u32FontColor       = _textColor;
  
  begin();
  
  setSwapBytes(true);				// used by push image function
	setRotation(u8Rotation);
  fillScreen(u32BackGroundColor);

  ScrollArea.createSprite(DISP_WIDTH - (2 * W_SPACE), 20);
  ScrollArea.setFreeFont(DEFAULT_FONT);
}       

// ---------------------------------------------------------------------------------------------------
// show frame
// ---------------------------------------------------------------------------------------------------
void cDisplay::showFrame(void) {
  fillScreen(TFT_BLACK);
  drawRoundRect(1, Y_TOP, DISP_WIDTH - 1, H_TOP, 10, TFT_BLUE);
  drawRoundRect(1, Y_MIDDLE, DISP_WIDTH - 1, H_MIDDLE, 10, TFT_BLUE);
  drawRoundRect(1, Y_BOTTOM, DISP_WIDTH - 1, H_BOTTOM, 10, TFT_BLUE);

  showInfoLine("WebRadio - ESP32 / VS1053");
}

// ---------------------------------------------------------------------------------------------------
// main window
// ---------------------------------------------------------------------------------------------------
void cDisplay::showMain(const char* _strData) {
  static char strOld[40] = {"                                       "};

  setFreeFont(DEFAULT_FONT);
	
  // clear actual string
  setTextColor(TFT_BLACK, TFT_BLACK);
  drawCentreString(strOld, DISP_WIDTH / 2, Y_BOTTOM + 8, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawCentreString(_strData, DISP_WIDTH / 2, Y_BOTTOM + 8, 1);
	
  strcpy(strOld, _strData);
}

// ---------------------------------------------------------------------------------------------------
// show station
// ---------------------------------------------------------------------------------------------------
void cDisplay::showStation(const station_t* _strData) {
  static char strOld[40] = {"                                       "};
  char strNew[40];

  snprintf_P(strNew, sizeof(strNew), PSTR("%i - %s"), _strData->Number, _strData->Name);

  setFreeFont(DEFAULT_FONT);
	
  // clear actual string
  setTextColor(TFT_BLACK, TFT_BLACK);
  drawString(strOld, W_SPACE, MAIN_LINE_1, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawString(strNew, W_SPACE, MAIN_LINE_1, 1);

  strcpy(strOld, strNew);
}

// ---------------------------------------------------------------------------------------------------
// section upper line
// ---------------------------------------------------------------------------------------------------
void cDisplay::showInfoLine(const char* _strData) {
  setFreeFont(DEFAULT_FONT);
	
  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawCentreString(_strData, DISP_WIDTH / 2, H_SPACE, 1);
}

// ---------------------------------------------------------------------------------------------------
// show volume
// ---------------------------------------------------------------------------------------------------
void cDisplay::showVolume(uint16_t _u16Vol) {
  static char strOld[4] = {"   "};
  static uint16_t u16FirstRun = 1;
  char strNew[4];

  snprintf_P(strNew, sizeof(strNew), PSTR("%i"), _u16Vol);

  setFreeFont(DEFAULT_FONT);
	
  // write start of line
  if (u16FirstRun == 1) {
    drawRightString("Vol.: ", DISP_WIDTH - (W_SPACE + 28), MAIN_LINE_1, 1);
    u16FirstRun = 0;
  }

  // clear actual string
  setTextColor(TFT_BLACK, TFT_BLACK);
  drawRightString(strOld, DISP_WIDTH - W_SPACE, MAIN_LINE_1, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawRightString(strNew, DISP_WIDTH - W_SPACE, MAIN_LINE_1, 1);

  strcpy(strOld, strNew);
}

// ---------------------------------------------------------------------------------------------------
// show actual bitrate
// ---------------------------------------------------------------------------------------------------
void cDisplay::showBitrate(String strData) {
	char cData[31];
	
  strData.toCharArray(cData, 30, 0);		

	setFreeFont(DEFAULT_FONT);
	
  // clear actual string
  setTextColor(TFT_BLACK, TFT_BLACK);
  drawString("                               ", W_SPACE, MAIN_LINE_2, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawString(cData, W_SPACE, MAIN_LINE_2, 1);
}

// ---------------------------------------------------------------------------------------------------
// show lower section
// ---------------------------------------------------------------------------------------------------
void cDisplay::showStatusLine(const char* _strData) {
	static char strOld[40] = {"                                       "};
			
	setFreeFont(DEFAULT_FONT);
	
  // clear actual string
	setTextColor(TFT_BLACK, TFT_BLACK);
	drawCentreString(strOld, DISP_WIDTH / 2, Y_BOTTOM + 8, 1);

	// draw new string
	setTextColor(TFT_YELLOW, TFT_BLACK);
	drawCentreString(_strData, DISP_WIDTH / 2, Y_BOTTOM + 8, 1);
	setFreeFont(NULL);

	strcpy(strOld, _strData);
}

// ---------------------------------------------------------------------------------------------------
// show song title
// ---------------------------------------------------------------------------------------------------
void cDisplay::showSongTitle(const char* _strData) {

  uint16_t u16TextWidth;
  bStopScroll = true;

  ScrollArea.setFreeFont(DEFAULT_FONT);

  strlcpy(strTitleNew, _strData, sizeof(strTitleNew));
  
  u16TextWidth = textWidth(_strData);
  
  if (u16TextWidth > 296) {
    setScrollStr(_strData, MAIN_LINE_6);
    bStopScroll = false;
  } else {
    ScrollArea.fillSprite(TFT_BLACK);
    
    // draw new string
    setTextColor(TFT_YELLOW, TFT_BLACK);
    ScrollArea.drawCentreString(strTitleNew, 150, 0, 1);
    ScrollArea.pushSprite(W_SPACE, MAIN_LINE_6);
  }
}

// ---------------------------------------------------------------------------------------------------
// cut string width to maximal pixel value
// ---------------------------------------------------------------------------------------------------
void cDisplay::truncateStringToWidth(const char* _str, char* _strResult, uint16_t _maxWidth) {
  uint16_t i = 0;
  actWidth = 0;
  charWidth = 0;
  
  charWidth = textWidth(_str);
  Serial.print(String(F("Textbreite org. - ")));
  Serial.println(charWidth);

  while (_str[i] != '\0') {
    charWidth = textWidth(String(_str[i]));

    actWidth += charWidth;

    if (actWidth > _maxWidth) {
      i--;
      Serial.print(String(F("Abbruch bei ")));
      Serial.println(actWidth);
      goto out; 
    } else {
       _strResult[i] = _str[i];
       i++;
    }      
  }

out:
  _strResult[i] = '\0';  
}

// ---------------------------------------------------------------------------------------------------
// set scroll text
// ---------------------------------------------------------------------------------------------------
void cDisplay::setScrollStr(const char* cString, uint16_t u16Line) {
  strcpy(strScroll, cString);
  strcat(strScroll, "  ***  ");         // put char to the end of the string
  
  u16LineNumber = u16Line;
  bNewStringToScroll = true;
}

// ---------------------------------------------------------------------------------------------------
// scroll text
// ---------------------------------------------------------------------------------------------------
//                       0                   30
// display               [-------------------]
//          0            x --->              y                   i - 1 
// char    [-------------[-------------------]-------------------]
//                            text scroll       
// ---------------------------------------------------------------------------------------------------
void cDisplay::scrollText(void) {
  static uint32_t u32Timer = 0;
  static uint16_t u16State = 0;
  static uint16_t u16Length = 0;
  
  static char cStrToShow[100];
  
  static uint16_t i = 0;

  if (bStopScroll) {
    return;
  }

  // check for new string
  if (bNewStringToScroll) {
    bNewStringToScroll = false;
    u16State = 0;
  }

  switch (u16State) {
    case 0:         // check for string length
      u16Length = strlen(strScroll);
      i = 0;
      u16State = 10;
      break;
    case 10:        // scroll text
      // clear actual string
      ScrollArea.fillSprite(TFT_BLACK);
      
      // calc new text to show
      if (i > u16Length) {
        i = 0;
      }

      strncpy(cStrToShow, &strScroll[i], 40);
      
      if (i > u16Length - 40) {
          strncpy(&cStrToShow[u16Length - i], strScroll, 40 - (u16Length - i));  
      }
     
      i++;
      
      // draw new string
      ScrollArea.setTextColor(TFT_YELLOW, TFT_BLACK);
      //ScrollArea.drawCentreString(cStrToShow, 150, 0, 1);
      ScrollArea.drawString(cStrToShow, 0, 0, 1);
      ScrollArea.pushSprite(W_SPACE, u16LineNumber);
      
      u32Timer = millis();
      u16State = 20;
      break;
    case 20:
        if (millis() - u32Timer >= 150) {
          u16State = 10;    
        }      
        break;
    default:
      break;  
  }
}

