/*
 * psOutput.cpp
 *
 *  Created on: 27.07.2022
 *      Author: pschneider
 */

#include "psGpio.h"

clOut::clOut(uint16_t _pin, POLARITY _polarity) {
	u16OutPin = _pin;
	Polarity = _polarity;
	pinMode(u16OutPin, OUTPUT);
	Off();
}

void clOut::Init(uint16_t _pin, POLARITY _polarity) {
	u16OutPin = _pin;
	Polarity = _polarity;
	pinMode(u16OutPin, OUTPUT);
	Off();
}

void clOut::SwPort(bool Status) {
	if (Status) { 
		On();
	} else { 
		Off();
	}
}

void clOut::On(void) {
	if (Polarity == POLARITY::POS) {
		digitalWrite(u16OutPin, HIGH);
	} else {
		digitalWrite(u16OutPin, LOW);
	}
	bStatus = true;
}

bool clOut::getState(void) {
	return bStatus;
}

void clOut::Off(void) {
	if (Polarity == POLARITY::POS) {
		digitalWrite(u16OutPin, LOW);
	} else {
		digitalWrite(u16OutPin, HIGH);
	}
	bStatus = false;
}

void clOut::Toggle(void) {
	if (bStatus) {
		Off();
		bStatus = false;
	} else {
		On();
		bStatus = true;
	}
}	

bool clOut::Flash(bool _Reset, uint16_t _flashTime) {
	bool bResult = false;

	if (!_Reset) {
		Off();
		u16FlashStatus = 0;
		return true;
	}

	switch (u16FlashStatus) {
		case 0:
			u32AktuelleZeit = millis();
			On();
			u16FlashStatus = 10;
			break;
		case 10:
			if (millis() - u32AktuelleZeit >= _flashTime) {
				Off();
				u16FlashStatus = 20;
			}
			break;
		case 20:			// wait for Reset false
			bResult = true;
			break;
		default:
			u16FlashStatus = 0;
			break;
	}
	return bResult;
}