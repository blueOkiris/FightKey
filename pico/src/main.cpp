/*
 * Author: Dylan Turner
 * Description: Entry point for Keyboard system
 */

#include <stdlib.h>
#include <pico/stdlib.h>
#include <bsp/board.h>
#include <map>
#include <vector>
#include <memory>
#include <Buttons.hpp>
#include <Controller.hpp>
#include <Hid.hpp>

using namespace fightkey;

const std::map<Button, uint> g_btnMapping = {
    { Button::Up,           0 },
    { Button::Down,         1 },
    { Button::Left,         2 },
    { Button::Right,        3 },
    { Button::Start,        4 },
    { Button::Select,       5 },
    { Button::LightPunch,   6 },
    { Button::MediumPunch,  7 },
    { Button::HeavyPunch,   8 },
    { Button::AllPunch,     9 },
    { Button::LightKick,    10 },
    { Button::MediumKick,   11 },
    { Button::HeavyKick,    12 },
    { Button::AllKick,      13 }
};
const std::map<Button, uint8_t> g_keyMapping = {
    { Button::Up,           HID_KEY_ARROW_UP },
    { Button::Down,         HID_KEY_ARROW_DOWN },
    { Button::Left,         HID_KEY_ARROW_LEFT },
    { Button::Right,        HID_KEY_ARROW_RIGHT },
    { Button::Start,        HID_KEY_ENTER },
    { Button::Select,       HID_KEY_SHIFT_LEFT },
    { Button::LightPunch,   HID_KEY_U },
    { Button::MediumPunch,  HID_KEY_I },
    { Button::HeavyPunch,   HID_KEY_O },
    { Button::AllPunch,     HID_KEY_P },
    { Button::LightKick,    HID_KEY_J },
    { Button::MediumKick,   HID_KEY_K },
    { Button::HeavyKick,    HID_KEY_L },
    { Button::AllKick,      HID_KEY_SEMICOLON }
};

const int pressToReleaseDelay = 30; // Minimum press down before an up registers

void readButtonsSendKeys(void); // Primary functionality
void testHid(void);
void testButtons(void);

void keyboardPressAndReleaseKey(const Button btn, HidKeyboard &keyboard) {
    keyboard.pressKey(btn);
    keyboard.update();
    keyboard.delayMs(pressToReleaseDelay);
    keyboard.releaseKey();
    keyboard.update();
    keyboard.delayMs(pressToReleaseDelay);
}

int main(void) {
    //readButtonsSendKeys();
    testHid();
    //testButtons();

    return 0;
}

void readButtonsSendKeys(void) {
    const GpioController controller(g_btnMapping);
    HidKeyboard keyboard(g_keyMapping);

    while(1) {
        const auto btnState = controller.state();
        for(const auto &btnStatePair : btnState) {
            if(btnStatePair.second) {
                keyboardPressAndReleaseKey(btnStatePair.first, keyboard);
            }
        }
    }
}

void testHid(void) {
    HidKeyboard keyboard(g_keyMapping);
    
    while(1) {
        keyboardPressAndReleaseKey(Button::Down, keyboard);
        keyboardPressAndReleaseKey(Button::Right, keyboard);
        keyboardPressAndReleaseKey(Button::Down, keyboard);
        keyboardPressAndReleaseKey(Button::Right, keyboard);
        keyboardPressAndReleaseKey(Button::LightKick, keyboard);

        keyboard.delayMs(1000);
    }
}

void testButtons(void) {

}
