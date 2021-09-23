/*
 * Author: Dylan Turner
 * Description: Definition of hid system
 */

#include <stddef.h>
#include <map>
#include <Buttons.hpp>

#pragma once

namespace fightkey {
    // Settings for LED blink based on hid state
    enum class BlinkDelay {
        Unmounted = 250,
        Mounted = 1000,
        Suspended = 2500
    };

    // A class that's used by UsbHid to blink an LED to show state
    class BlinkLed {
        public:
            void updateTask(void);

            static BlinkDelay curBlinkIntval;

        private:
            uint32_t _startMs;
            bool _ledState;
    };

    // The actual keyboard system
    class HidKeyboard {
        public:
            HidKeyboard(const std::map<Button, uint8_t> &keyMappings);

            void update(void);
            void pressKey(const Button btn);
            void releaseKey(void);
            bool isKeyPressed(const Button btn);
        
        private:
            std::map<Button, uint8_t> _keyMappings;

            const int _pollIntervalMs;
            BlinkLed _led;

            uint32_t _startMs;
            bool _hasKey, _toggle, _release;
            uint8_t _key;
    };
}
