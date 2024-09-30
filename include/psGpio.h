/*
 * psGPIO.h
 *
 *  Created on: 24.06.2020
 *      Author: pschneider
 */

#pragma once

#include <Arduino.h>

enum class POLARITY: uint16_t {
	NEG = 0,
	POS = 1
};

// ---------------------------------------------------------------------------------------------------
// Output
// ---------------------------------------------------------------------------------------------------
class clOut {
	public:
		clOut(uint16_t pin, POLARITY polarity);
		~clOut(){}

		void Init(uint16_t pin, POLARITY polarity);
		void SwPort(bool Status);
		void On(void);
		void Off(void);
		void Toggle(void);
		bool Flash(bool Reset, uint16_t flashTime);
		bool getState(void);
	
	private:
		bool     bStatus = false;
		uint32_t u32AktuelleZeit = 0;
		uint16_t u16OutPin = 0;
		uint16_t u16FlashStatus = 0;
		POLARITY Polarity = POLARITY::POS;
};

// ---------------------------------------------------------------------------------------------------
// Input
// ---------------------------------------------------------------------------------------------------
class clIn {
	public:
		clIn(uint16_t _pin, uint16_t _mode, uint32_t _entprellzeit, 
		     uint32_t _switchLongTime, void (*cb)(void), POLARITY _polarity);
		
		clIn(uint16_t _pin, uint32_t _entprellzeit, uint32_t _switchLongTime, POLARITY _polarity); 	 
		
		~clIn(){}

		void incValue(uint16_t *, uint16_t, uint16_t, bool, bool);
		void decValue(uint16_t *, uint16_t, uint16_t, bool, bool);
		void runState(void);
		
		bool Status(void) {return bShort;}
		bool StatusLong(void) {return bLong;}

	private:
		bool GetStatus(void);
		void initData(void);
	
		uint32_t u32Timer;
	 	
		uint16_t u16Status;
		uint16_t u16StatusOld;

		uint16_t u16IncValueState;
		uint16_t u16DecValueState;

		uint16_t u16InCount;
		static uint16_t u16InCountMax;
				
		bool bShort;
		bool bLong;

		uint32_t u32Entprellzeit;
   	uint32_t u32SwitchLongTime;
		uint32_t u32TimerInc;
		uint32_t u32TimerDec;
   	POLARITY polarity;
   	uint16_t u16Mode;
		uint16_t u16Pin; 
};

