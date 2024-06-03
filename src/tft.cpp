/*
 * tft.cpp
 *
 *  Created on: 02.05.2024
 *      Author: Peter Schneider
 */

#include <TFT_eSPI.h>
#include "tft.h"

cDisplay::cDisplay(uint8_t _rotation, uint32_t _backGroundColor, uint32_t _textColor) {
  u8Rotation         = _rotation;
  u32BackGroundColor = _backGroundColor;
  u32FontColor       = _textColor;
  
  begin();
  
  setSwapBytes(true);				// used by push image function
	setRotation(u8Rotation);
  fillScreen(u32BackGroundColor);
}       

// ---------------------------------------------------------------------------------------------------
// show frame
// ---------------------------------------------------------------------------------------------------
void cDisplay::showFrame(void) {
  fillScreen(TFT_BLACK);
  drawRoundRect(1, Y_TOP, DISP_WIDTH - 1, H_TOP, 10, TFT_BLUE);
  drawRoundRect(1, Y_MIDDLE, DISP_WIDTH - 1, H_MIDDLE, 10, TFT_BLUE);
  drawRoundRect(1, Y_BOTTOM, DISP_WIDTH - 1, H_BOTTOM, 10, TFT_BLUE);

  showUpperSection("WebRadio - ESP32 / VS1053");
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
  drawString(strOld, W_SPACE, Y_MIDDLE + H_SPACE, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawString(strNew, W_SPACE, Y_MIDDLE + H_SPACE, 1);

  strcpy(strOld, strNew);
}

// ---------------------------------------------------------------------------------------------------
// owerSection upper line
// ---------------------------------------------------------------------------------------------------
void cDisplay::showUpperSection(const char* _strData) {
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
    drawRightString("Vol.: ", DISP_WIDTH - (W_SPACE + 28), Y_MIDDLE + H_SPACE, 1);
    u16FirstRun = 0;
  }

  // clear actual string
  setTextColor(TFT_BLACK, TFT_BLACK);
  drawRightString(strOld, DISP_WIDTH - W_SPACE, Y_MIDDLE + H_SPACE, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawRightString(strNew, DISP_WIDTH - W_SPACE, Y_MIDDLE + H_SPACE, 1);

  strcpy(strOld, strNew);
}

// ---------------------------------------------------------------------------------------------------
// show lower section
// ---------------------------------------------------------------------------------------------------
void cDisplay::showLowerSection(const char* _strData) {
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

  setFreeFont(DEFAULT_FONT);

  strlcpy(strTitleNew, _strData, sizeof(strTitleNew));
  
  u16TextWidth = textWidth(_strData);
  Serial.println(_strData);

  if (u16TextWidth > 296) {
    truncateStringToWidth(_strData, strTitleNew, 250);//Y_PLACE);
    u16TextWidth = textWidth(strTitleNew);
    Serial.print(String(F("Textbreite - ")));
    Serial.println(u16TextWidth);
  }

  Serial.println(strTitleNew);
  
  // clear actual string
  setTextColor(TFT_BLACK, TFT_BLACK);
  drawCentreString(strTitleOld, DISP_WIDTH / 2, Y_MIDDLE + H_MIDDLE - 3 * H_SPACE, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawCentreString(strTitleNew, DISP_WIDTH / 2, Y_MIDDLE + H_MIDDLE - 3 * H_SPACE, 1);
  
  strlcpy(strTitleOld, strTitleNew, sizeof(strTitleOld));
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
// scroll text
// ---------------------------------------------------------------------------------------------------
void cDisplay::scrollText(const char* _myString, uint16_t _scrollWidth, uint16_t _scrollSpeed, uint16_t _textWidth, uint16_t _xPos) {
  static uint16_t u16State = 0;
  static uint16_t startX = 0;
  static uint16_t i = 0;

  uint16_t charWidth = 0;
  char currentChar;

  switch (u16State) {
    case 0:         // int scroll area
      fillRect(0, 0, _scrollWidth, fontHeight(), TFT_BLACK);
      startX = _scrollWidth;     // set start position of string to scroll
      i = 0;
      u16State = 10;
      break;
    case 10:
      currentChar = _myString[i];
      charWidth = textWidth(String(currentChar));
      if (startX + charWidth > 0 && startX < _scrollWidth) {
        setCursor(startX, 0);
        print(currentChar);
      }
    
      startX += charWidth;
      if (startX >= _scrollWidth) {
        u16State = 20;
      }
      break;
    case 20:
      _xPos -= _scrollSpeed;

      // Wenn der Text vollständig aus dem sichtbaren Bereich herausgelaufen ist, 
      // setze die x-Position zurück
      if (_xPos + _textWidth < 0) {
        _xPos = _scrollWidth;
      }
      break;  
  }
}

