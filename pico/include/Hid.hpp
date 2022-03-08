/*
 * Author: Dylan Turner
 * Description: Definition of hid system
 */

#include <stddef.h>
#include <map>
#include <vector>
#include <utility>
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
            void releaseKey(const Button btn);
            void delayMs(int delay);
            void delayUs(int delay);
            bool isKeyPressed(const Button btn);
        
        private:
            std::map<Button, uint8_t> _keyMappings;

            const int _pollIntervalMs;
            BlinkLed _led;

            uint32_t _startMs;
            bool _change;
            std::vector<std::pair<Button, bool>> _pressedKeys;
    };

    // Alternative to keybaord that supports GamePad inputs
    class HidGamepad {
        public:
            HidGamepad(void); // No alternate mappings

            void update(void);

            void pressButton(const Button btn);
            void releaseButton(const Button btn);

            void delayMs(int delay);
            void delayUs(int delay);
        
        private:
            const int _pollIntervalMs;
            BlinkLed _led;
            uint32_t _startMs;

            char _up, _down, _left, _right;
            hid_gamepad_report_t _report;
    };
}
