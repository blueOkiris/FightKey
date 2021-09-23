/*
 * Author: Dylan Turner
 * Description: Implementation of hid system
 */

#include <stddef.h>
#include <stdio.h>
#include <tusb.h>
#include <bsp/board.h>
#include <usb_descriptors.h>
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
        _hasKey(false), _toggle(false), _release(false),
        _key(0) {
    board_init();
    tusb_init();
}

void HidKeyboard::releaseKey(void) {
    _release = true;
}

void HidKeyboard::pressKey(const Button btn) {
    _key = _keyMappings[btn];
    _release = false; // Force a release
}

bool HidKeyboard::isKeyPressed(const Button btn) {
    return (_key == _keyMappings[btn]) && !_release;
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
        if(_key != 0 && (_toggle = !_toggle) && !_hasKey) {
            uint8_t keycode[6] = { 0 };
            keycode[0] = _key;

            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
            
            _hasKey = true;
            _key = 0;
        } else if(_release) {
            // send empty key report if previously has key pressed
            if(_hasKey) {
                tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                board_delay(10);
            }
            _hasKey = 0;
        }
    }
}
