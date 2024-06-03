/*
 * tft.h
 *
 *  Created on: 02.05.2024
 *      Author: Peter Schneider
 */

#pragma once

// x|y
// 0|0                                            320|0
// ----------------------------------------------------			0
//      UpperArea				35
// ----------------------------------------------------			35
// ----------------------------------------------------			43
//
//
//      MainArea   			154
//
//
// ----------------------------------------------------			197
// ----------------------------------------------------			205			
//      LowerArea				35
// ----------------------------------------------------			240
// 0|240                                        320|240

#include "Free_Fonts.h"

// to rotate the screen
#define ROTATION_NO  0
#define ROTATION_90  1
#define ROTATION_180 2
#define ROTATION_270 3

#define DEFAULT_FONT	FF17

typedef struct {
  uint16_t Number;
  char     Name[40]; 
  char     Url[100];
} station_t;

const uint16_t DISP_HIGTH = 240;
const uint16_t DISP_WIDTH = 320;

const uint16_t H_SPACE = 8;				// higth space
const uint16_t W_SPACE = 12;      // width space 
const uint16_t Y_TOP   = 0;		 	  // start upper area
const uint16_t H_TOP   = 35;		  // higth of upper area
const uint16_t Y_PLACE = DISP_WIDTH - 2 * W_SPACE;

const uint16_t H_BOTTOM = 35; 
const uint16_t Y_BOTTOM = DISP_HIGTH - H_BOTTOM;	// start lower area

const uint16_t Y_MIDDLE = H_TOP + H_SPACE;   	    // start middle area
const uint16_t H_MIDDLE = (DISP_HIGTH - H_TOP - H_BOTTOM) - (2 * H_SPACE);  // higth of middle area

class cDisplay : public TFT_eSPI {
	public:
		cDisplay(uint8_t, uint32_t, uint32_t);
    ~cDisplay(){}

		void showSongTitle(const char*);
		void showUpperSection(const char*);
		void showLowerSection(const char*);
		void showFrame(void);
		void showMain(const char*);
		void showStation(const station_t*);
		void showVolume(uint16_t);
		void scrollText(const char*, uint16_t, uint16_t, uint16_t, uint16_t);

		
	private:
		void truncateStringToWidth(const char*, char*, uint16_t);

		uint8_t  u8Rotation;
  	uint32_t u32BackGroundColor;
  	uint32_t u32FontColor;

	  uint16_t actWidth;
	  uint16_t charWidth;

		char strTitleOld[100];
  	char strTitleNew[100];
};

