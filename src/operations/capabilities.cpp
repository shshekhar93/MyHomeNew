#include "capabilities.h"
#include "Arduino.h"

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
        digitalWrite(m_pinIds[i], HIGH);
    }
}

bool MyHomeNew::Capabilities::setState(uint8_t pin, bool state, float pwm) {
    Serial.printf("switched %s pin %d", (state? "ON": "OFF"), pin);
    digitalWrite(m_pinIds[pin], state ? LOW : HIGH);
    return true;
}
