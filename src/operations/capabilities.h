/**
 * Capability flags:
 * 0x01: Lead 1
 * 0x02: Lead 2
 * 0x04: Lead 3
 * 0x08: Lead 4
 * 0x10: Lead 5
 * 0x20: Lead 6
 * 0x40: Lead 7
 * 0x80: Lead 8
 * FOUR_LEADS: Lead 1 | Lead 2 | Lead 3 | Lead 4
 * EIGHT_LEADS: FOUR_LEADS | Lead 5 | Lead 6 | Lead 7 | Lead 8
 * REMOTE: 
 */
#include "os_type.h"
#include "Arduino.h"

namespace MyHomeNew {
    enum CapabilitiesEnum {
        FOUR_LEADS,
        EIGHT_LEADS,
        REMOTE
    };

    class Capabilities {
        private:
            static uint8_t m_pinIds[4];
            static uint8_t m_numLeads;
        public:
            static void setPinMode();
            static uint8_t getState(uint8_t pin);
            static bool setState(uint8_t pin, uint8_t dutyPercent);
            static void switchOn(uint8_t);
            static void switchOff(uint8_t);
            static void switchWithPWM(uint8_t, uint8_t);
    };
}
