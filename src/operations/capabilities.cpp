#include "capabilities.h"
#include "Arduino.h"
#include "../config/Config.h"

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
        String savedState = Config::getInstance()->getValue((ConfigKeys)(CONFIG_LEAD1 + i));
        digitalWrite(m_pinIds[i], savedState == "on" ? LOW : HIGH);
    }
}

bool MyHomeNew::Capabilities::getState(uint8_t pin) {
    return digitalRead(m_pinIds[pin]) == LOW;
}

bool MyHomeNew::Capabilities::setState(uint8_t pin, bool state, float pwm) {
    digitalWrite(m_pinIds[pin], state ? LOW : HIGH);
    Config::getInstance()->setValue((ConfigKeys)(CONFIG_LEAD1 + pin), state ? "on": "off")->save();
    return true;
}
