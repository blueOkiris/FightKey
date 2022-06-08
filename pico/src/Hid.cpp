/*
 * Author: Dylan Turner
 * Description: Implementation of hid system
 */

#include <stddef.h>
#include <stdio.h>
#include <tusb.h>
#include <bsp/board.h>
#include <usb_descriptors.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include <vector>
#include <utility>
#include <map>
#include <Hid.hpp>

using namespace fightkey;

/* Blink stuff */

BlinkDelay BlinkLed::curBlinkIntval = BlinkDelay::Unmounted;

void BlinkLed::updateTask(void) {
    // Blink at every interval ms
    if(board_millis() - _startMs < static_cast<uint32_t>(curBlinkIntval)) {
        // Not enough time yet
        return;
    }
    _startMs += static_cast<uint32_t>(curBlinkIntval);

    board_led_write(_ledState);
    _ledState = !_ledState;
}

// Device callbacks
// Invoked when device is mounted
void tud_mount_cb(void) {
    BlinkLed::curBlinkIntval = BlinkDelay::Mounted;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    BlinkLed::curBlinkIntval = BlinkDelay::Mounted;
}

/*
 * Invoked when usb bus is suspended
 * remote_wakeup_en : if host allow us  to perform remote wakeup
 * Within 7ms, device must draw an average of current less than 2.5 mA from bus
 */
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
    BlinkLed::curBlinkIntval = BlinkDelay::Suspended;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
    BlinkLed::curBlinkIntval = BlinkDelay::Mounted;
}

/* Keyboard stuff */

HidKeyboard::HidKeyboard(const std::map<Button, uint8_t> &keyMappings) :
        _keyMappings(keyMappings),
        _pollIntervalMs(10), _startMs(0),
        _change(false) {
    board_init();
    tusb_init();
}

void HidKeyboard::delayMs(int delay) {
    while(delay > 0) {
        update();
        sleep_ms(1);
        delay--;
    }
}

void HidKeyboard::delayUs(int delay) {
    while(delay > 0) {
        update();
        sleep_us(1);
        delay--;
    }
}

void HidKeyboard::releaseKey(const Button btn) {
    for(int i = 0; i < _pressedKeys.size(); i++) {
        if(_pressedKeys[i].first == btn) {
            _pressedKeys[i].second = false;
            _change = true;
        }
    }
}

void HidKeyboard::pressKey(const Button btn) {
    for(const auto &btnStatePair : _pressedKeys) {
        if(btnStatePair.first == btn) {
            return;
        }
    }
    _change = true;
    _pressedKeys.push_back(std::make_pair(btn, true));
}

bool HidKeyboard::isKeyPressed(const Button btn) {
    for(const auto &btnStatePair : _pressedKeys) {
        if(btnStatePair.first == btn) {
            return btnStatePair.second;
        }
    }
    return false;
}

void HidKeyboard::update(void) {
    // HID library stuff
    tud_task(); // Device task for usb
    _led.updateTask(); // Show device status

    // Not enough time yet
    if(board_millis() - _startMs < _pollIntervalMs) {
        return;
    }
    _startMs += _pollIntervalMs;

    // Remote wakeup
    if(tud_suspended()) {
        /*
         * Wake up host if we are in suspend mode
         * and REMOTE_WAKEUP feature is enabled by host
         */
        tud_remote_wakeup();
    }

    // Keyboard control
    if (tud_hid_ready()) {
        if(_change) {
            uint8_t keycode[6] = { 0, 0, 0, 0, 0, 0 };
            for(int i = 0; i < _pressedKeys.size() && i < 6; i++) {
                if(_pressedKeys[i].second) {
                    // Add press
                    keycode[i] = _keyMappings[_pressedKeys[i].first];
                } else {
                    // Send release
                    keycode[i] = 0;
                    _pressedKeys.erase(_pressedKeys.begin() + i);
                    i--;
                }
            }

            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
            board_delay(10);

            _change = false;
        }
    }
}

/* Gamepad stuff */

HidGamepad::HidGamepad() :
        _pollIntervalMs(10), _startMs(0),
        _up(0), _down(0), _left(0), _right(0), _lt(0), _rt(0),
        _report(hid_gamepad_report_t {
            0, 0, 0, 0, 0, 0, GAMEPAD_HAT_CENTERED, 0
        }) {
    board_init();
    tusb_init();
}

void HidGamepad::delayMs(int delay) {
    while(delay > 0) {
        update();
        sleep_ms(1);
        delay--;
    }
}

void HidGamepad::delayUs(int delay) {
    while(delay > 0) {
        update();
        sleep_us(1);
        delay--;
    }
}

void HidGamepad::pressButton(const Button btn) {
    switch(btn) {
        // Hat buttons
        case Button::Up:
            _up = 1;
            break;
        case Button::Down:
            _down = 1;
            break;
        case Button::Left:
            _left = 1;
            break;
        case Button::Right:
            _right = 1;
            break;

        // Specific buttons
        case Button::LightPunch:
            _report.buttons |= GAMEPAD_BUTTON_NORTH; // Actually X
            break;
        case Button::MediumPunch:
            _report.buttons |= GAMEPAD_BUTTON_WEST; // Actually Y
            break;
        case Button::HeavyPunch:
            _report.buttons |= GAMEPAD_BUTTON_TR;
            break;
        case Button::HeavyKick:
            _report.buttons |= GAMEPAD_BUTTON_TL;
            break;
        case Button::LightKick:
            _report.buttons |= GAMEPAD_BUTTON_SOUTH; // A
            break;
        case Button::MediumKick:
            _report.buttons |= GAMEPAD_BUTTON_EAST; // B
            break;
        case Button::Start:
            _report.buttons |= GAMEPAD_BUTTON_START;
            break;
        case Button::Select:
            _report.buttons |= GAMEPAD_BUTTON_SELECT;
            break;

        // Triggers
        case Button::AllPunch:
            _report.rz = 127;
            break;
        case Button::AllKick:
            _report.z = 127;
            break;
    }
}

void HidGamepad::releaseButton(const Button btn) {
    switch(btn) {
        // Hat buttons
        case Button::Up:
            _up = 0;
            break;
        case Button::Down:
            _down = 0;
            break;
        case Button::Left:
            _left = 0;
            break;
        case Button::Right:
            _right = 0;
            break;

        // Specific buttons
        case Button::LightPunch:
            _report.buttons &= ~GAMEPAD_BUTTON_NORTH; // X
            break;
        case Button::MediumPunch:
            _report.buttons &= ~GAMEPAD_BUTTON_WEST; // Y
            break;
        case Button::HeavyPunch:
            _report.buttons &= ~GAMEPAD_BUTTON_TR;
            break;
        case Button::HeavyKick:
            _report.buttons &= ~GAMEPAD_BUTTON_TL;
            break;
        case Button::LightKick:
            _report.buttons &= ~GAMEPAD_BUTTON_SOUTH; // A
            break;
        case Button::MediumKick:
            _report.buttons &= ~GAMEPAD_BUTTON_EAST; // B
            break;
        case Button::Start:
            _report.buttons &= ~GAMEPAD_BUTTON_START;
            break;
        case Button::Select:
            _report.buttons &= ~GAMEPAD_BUTTON_SELECT;
            break;

        // Triggers
        case Button::AllPunch:
            _report.rz = -128;
            break;
        case Button::AllKick:
            _report.z = -128;
            break;
    }
}

void HidGamepad::update(void) {
    // HID library stuff
    tud_task(); // Device task for usb
    _led.updateTask(); // Show device status

    // Not enough time yet
    if(board_millis() - _startMs < _pollIntervalMs) {
        return;
    }
    _startMs += _pollIntervalMs;

    // Remote wakeup
    if(tud_suspended()) {
        /*
         * Wake up host if we are in suspend mode
         * and REMOTE_WAKEUP feature is enabled by host
         */
        tud_remote_wakeup();
    }

    // Keyboard control
    if (tud_hid_ready()) {
        if(_up && _down && _left && _right) {
            _report.hat = GAMEPAD_HAT_CENTERED;
        } else if(_down && _left && _right) {
            _report.hat = GAMEPAD_HAT_DOWN;
        } else if(_down && _left) {
            _report.hat = GAMEPAD_HAT_DOWN_LEFT;
        } else if(_down && _right) {
            _report.hat = GAMEPAD_HAT_DOWN_RIGHT;
        } else if(_down) {
            _report.hat = GAMEPAD_HAT_DOWN;
        } else if(_up && _left && _right) {
            _report.hat = GAMEPAD_HAT_UP;
        } else if(_up && _left) {
            _report.hat = GAMEPAD_HAT_UP_LEFT;
        } else if(_up && _right) {
            _report.hat = GAMEPAD_HAT_UP_RIGHT;
        } else if(_up) {
            _report.hat = GAMEPAD_HAT_UP;
        } else if(_left && _right) {
            _report.hat = GAMEPAD_HAT_CENTERED;
        } else if(_left) {
            _report.hat = GAMEPAD_HAT_LEFT;
        } else if(_right) {
            _report.hat = GAMEPAD_HAT_RIGHT;
        } else {
            _report.hat = GAMEPAD_HAT_CENTERED;
        }

        tud_hid_gamepad_report(
            REPORT_ID_GAMEPAD,
            _report.x, _report.y, _report.z, _report.rz,
            _report.rx, _report.ry, _report.hat, _report.buttons
        );
        board_delay(10);
    }
}
