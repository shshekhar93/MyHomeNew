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
    for(uint8_t i = 0; i < m_numLeads; i++) {
        pinMode(m_pinIds[i], OUTPUT);
        uint8_t dutyPercent = Config::getInstance()->getLeadVal((ConfigKeys)(CONFIG_LEAD1 + i));

        if(dutyPercent <= 0) {
            return digitalWrite(m_pinIds[i], HIGH);
        }

        if(dutyPercent >= 100) {
            return digitalWrite(m_pinIds[i], LOW);
        }

        return analogWrite(m_pinIds[i], (int)(10.23 * (100 - dutyPercent)));
    }
}

uint8_t MyHomeNew::Capabilities::getState(uint8_t pin) {
    return Config::getInstance()->getLeadVal((ConfigKeys)(CONFIG_LEAD1 + pin));
}

bool MyHomeNew::Capabilities::setState(uint8_t pin, uint8_t dutyPercent) {
    if(dutyPercent <= 0) {
        digitalWrite(m_pinIds[pin], HIGH);
        dutyPercent = 0;
    }
    else if(dutyPercent >= 100) {
        digitalWrite(m_pinIds[pin], LOW);
        dutyPercent = 100;
    }
    else
    {
        analogWrite(m_pinIds[pin], (int)(10.23 * (100 - dutyPercent)));
    }

    Config::getInstance()->setLeadVal((ConfigKeys)(CONFIG_LEAD1 + pin), dutyPercent)->save();
    return true;
}
