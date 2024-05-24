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
// show song tetle
// ---------------------------------------------------------------------------------------------------
void cDisplay::showSongTitle(const char* _strData) {
  static char strOld[30] = {"                             "};
  static char strNew[30] = {"                             "};
  int i = 0;

  while (_strData[i] != '\0') {
    if (i >= 30) {
      strNew[29] = '\0';
      goto out;
    } else {
      strNew[i] = _strData[i];
    }
    i++;
  }

out:
  setFreeFont(DEFAULT_FONT);
	
  // clear actual string
  setTextColor(TFT_BLACK, TFT_BLACK);
  drawCentreString(strOld, DISP_WIDTH / 2, Y_MIDDLE + H_MIDDLE - 3 * H_SPACE, 1);

  // draw new string
  setTextColor(TFT_YELLOW, TFT_BLACK);
  drawCentreString(strNew, DISP_WIDTH / 2, Y_MIDDLE + H_MIDDLE - 3 * H_SPACE, 1);
  
  strcpy(strOld, strNew);
}

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
