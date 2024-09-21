/*
 * psInput.cpp
 *
 *  Created on: 04.04.2022
 *      Author: pschneider
 */

#include "psGpio.h"

uint16_t clIn::u16InCountMax = 0;

clIn::clIn(uint16_t _pin, uint16_t _mode, uint32_t _entprellzeit, 
           uint32_t _switchLongTime, void (*cb)(void), POLARITY _polarity) {
    
    u16InCountMax++;
    u16InCount = u16InCountMax;

    u32Entprellzeit = _entprellzeit;
    u32SwitchLongTime = _switchLongTime;
    polarity = _polarity;
    u16Mode = _mode;
    u16Pin = _pin;

    pinMode(u16Pin, INPUT);

    if (cb != nullptr) {
        noInterrupts();   // disable irq 
        if (polarity == POLARITY::POS) {
            attachInterrupt(digitalPinToInterrupt(u16Pin), cb, u16Mode);
        } else {
            attachInterrupt(digitalPinToInterrupt(u16Pin), cb, u16Mode);
        }
        interrupts();     // enable irq
    }

	initData();
}    

clIn::clIn(uint16_t _pin, uint32_t _entprellzeit, uint32_t _switchLongTime, POLARITY _polarity) {
    u16InCountMax++;
    u16InCount = u16InCountMax;

    u32Entprellzeit = _entprellzeit;
    u32SwitchLongTime = _switchLongTime;
    polarity = _polarity;
    u16Pin = _pin;

    initData();

    pinMode(u16Pin, INPUT);
}

void clIn::initData(void) {
    u16Status = 0;
    u16StatusOld = 1;

    u16IncValueState = 0;
    u16DecValueState = 0;
    
    bShort = false;
    bLong = false;
}

void clIn::runState(void) {

    if (u16Status != u16StatusOld) {
        Serial.println(String("Input " + String(u16InCount) + String(" Status - " + String(u16Status))));
        u16StatusOld = u16Status;
    }

    switch (u16Status) {
        case 0:
            bShort = false;
            bLong = false;
            if (GetStatus() == true) {
                u32Timer = millis();
                u16Status = 10;
            }
            break;
        case 10:
            if (GetStatus() == false) {
                u16Status = 0;
            } else if (millis() > (u32Timer + u32Entprellzeit)) {
                bShort = true;
                u32Timer = millis();
                u16Status = 20;	
            }					
            break;
        case 20:
            if (GetStatus() == false) {
                u16Status = 0;	
            } else if (millis() > (u32Timer + u32SwitchLongTime)) {
                bLong = true;
                u16Status = 30;
            }
            break;
        case 30:
           if (GetStatus() == false) {
                u16Status = 0;	
            }
            break;
        default:
            u16Status = 0;
            break;
    }
}

bool clIn::GetStatus(void) {
    if (polarity == POLARITY::POS) {
        return digitalRead(u16Pin);
    } else {
        return !digitalRead(u16Pin);
    }
}

void clIn::incValue(uint16_t *_pValue, uint16_t _valueMax, uint16_t _step, bool bStartAt0) {
    runState();

    switch (u16IncValueState) {
        case 0:    
            if (Status()) {   
                *_pValue = *_pValue + _step;
                if (*_pValue > _valueMax) {
                    if (bStartAt0) {
                        *_pValue = 0;
                    } else { 
                        *_pValue = _valueMax;
                    }                
                }
                u16IncValueState = 5;
            } 
            break;
        case 5:
            if (!Status()) {
                u16IncValueState = 0;
            }  
            break;
    
        default:
            u16IncValueState = 0;
            break; 
    }
}

void clIn::decValue(uint16_t *_pValue, uint16_t _valueMax, uint16_t _step, bool _bStartAtMax) {
    runState();
    
    switch (u16DecValueState) {
        case 0:    
            if (Status()) {   
                if (*_pValue > 0) {
                    *_pValue = *_pValue - _step;
                } else if (*_pValue == 0) {
                    if (_bStartAtMax) {
                        *_pValue = _valueMax;
                    }
                }
                u16DecValueState = 5;
            } 
            break;
        case 5:
            if (!Status()) {
                u16DecValueState = 0;
            }  
            break;
    
        default:
            u16DecValueState = 0;
            break; 
    }
}
