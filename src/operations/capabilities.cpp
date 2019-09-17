#include "capabilities.h"
#include "Arduino.h"
#include "../config/Config.h"
#include "../common/utils.h"

#ifdef __MY_HOME_EIGHT_LEADS_DEVICE__
    uint8_t MyHomeNew::Capabilities::m_numLeads = 8;
#else
    uint8_t MyHomeNew::Capabilities::m_numLeads = 4;
#endif

uint8_t MyHomeNew::Capabilities::m_pinIds[8] = { D0, D1, D2, D3, D4, D5, D6, D7 };

void MyHomeNew::Capabilities::resetLabels() {
    for(uint8_t i = 0; i < 8; i++)
        m_labels[i] = NULL;
}

void MyHomeNew::Capabilities::setOutputMode() {
    for(uint8_t pin = 0; pin < m_numLeads; pin++) {
        pinMode(m_pinIds[pin], OUTPUT);
        uint8_t dutyPercent = Config::getInstance()->getLeadVal((ConfigKeys)(CONFIG_LEAD1 + pin));

        if(dutyPercent <= 0) {
            switchOff(pin);
            continue;
        }

        if(dutyPercent >= 100) {
            switchOn(pin);
            continue;
        }

        switchWithPWM(pin, dutyPercent);
        continue;
    }
}

uint8_t MyHomeNew::Capabilities::getState(uint8_t pin) {
    return Config::getInstance()->getLeadVal((ConfigKeys)(CONFIG_LEAD1 + pin));
}

bool MyHomeNew::Capabilities::setState(uint8_t pin, uint8_t dutyPercent) {
    if(pin >= m_numLeads) {
        return false;
    }

    if(dutyPercent <= 0) {
        switchOff(pin);
        dutyPercent = 0;
    }
    else if(dutyPercent >= 100) {
        switchOn(pin);
        dutyPercent = 100;
    }
    else
    {
        switchWithPWM(pin, dutyPercent);
    }

    Config::getInstance()->setLeadVal((ConfigKeys)(CONFIG_LEAD1 + pin), dutyPercent)->save();
    return true;
}


void MyHomeNew::Capabilities::switchOn(uint8_t pin) {
    if(Config::getInstance()->isActiveStateLow()) {
        digitalWrite(m_pinIds[pin], LOW);
    } else {
        digitalWrite(m_pinIds[pin], HIGH);
    }
}

void MyHomeNew::Capabilities::switchOff(uint8_t pin) {
    if(Config::getInstance()->isActiveStateLow()) {
        digitalWrite(m_pinIds[pin], HIGH);
    } else {
        digitalWrite(m_pinIds[pin], LOW);
    }
}

void MyHomeNew::Capabilities::switchWithPWM(uint8_t pin, uint8_t dutyPercent) {
    if(Config::getInstance()->isActiveStateLow()) {
        dutyPercent = 100 - dutyPercent;
    }
    analogWrite(m_pinIds[pin], (int)(10.23 * dutyPercent));
}
