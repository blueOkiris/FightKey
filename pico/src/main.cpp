/*
 * Author: Dylan Turner
 * Description: Entry point for Keyboard system
 */

#include <stdio.h>
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
    { Button::Up,           2 },
    { Button::Down,         3 },
    { Button::Left,         4 },
    { Button::Right,        5 },
    { Button::Start,        7 },
    { Button::Select,       8 },
    { Button::LightPunch,   9 },
    { Button::MediumPunch,  10 },
    { Button::HeavyPunch,   11 },
    { Button::AllPunch,     12 },
    { Button::LightKick,    13 },
    { Button::MediumKick,   14 },
    { Button::HeavyKick,    15 },
    { Button::AllKick,      16 }
};
const std::map<Button, uint8_t> g_keyMapping = {
    { Button::Up,           HID_KEY_ARROW_UP },
    { Button::Down,         HID_KEY_ARROW_DOWN },
    { Button::Left,         HID_KEY_ARROW_LEFT },
    { Button::Right,        HID_KEY_ARROW_RIGHT },
    { Button::Start,        HID_KEY_ENTER },
    { Button::Select,       HID_KEY_SHIFT_LEFT },
    { Button::LightPunch,   HID_KEY_1 },
    { Button::MediumPunch,  HID_KEY_2 },
    { Button::HeavyPunch,   HID_KEY_3 },
    { Button::AllPunch,     HID_KEY_4 },
    { Button::LightKick,    HID_KEY_5 },
    { Button::MediumKick,   HID_KEY_6 },
    { Button::HeavyKick,    HID_KEY_7 },
    { Button::AllKick,      HID_KEY_8 },

    { Button::Escape,       HID_KEY_ESCAPE }
};

const int pressToReleaseDelay = 30; // Minimum press down before an up registers

void readButtonsSendKeys(void); // Primary functionality
void testHid(void);
void testButton(const Button btnToTest);

int main(void) {
    readButtonsSendKeys();
    //testHid();
    //testButton(Button::Up);

    return 0;
}

void readButtonsSendKeys(void) {
    const GpioController controller(g_btnMapping);
    HidKeyboard keyboard(g_keyMapping);

    Button heldBtn = Button::Up;

    while(1) {
        keyboard.update();

        const auto btnState = controller.state();
        for(const auto &btnStatePair : btnState) {
            if(btnStatePair.second) {
                keyboard.pressKey(btnStatePair.first);
                if(btnStatePair.first == Button::Select) {
                    keyboard.pressKey(Button::Escape);
                }
            } else {
                keyboard.releaseKey(btnStatePair.first);
            }
        }
    }
}

void keyboardPressAndReleaseKey(const Button btn, HidKeyboard &keyboard) {
    keyboard.pressKey(btn);
    keyboard.update();
    keyboard.delayMs(pressToReleaseDelay);
    keyboard.releaseKey(btn);
    keyboard.update();
    keyboard.delayMs(pressToReleaseDelay);
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

void testButton(const Button btnToTest) {
    const GpioController controller(g_btnMapping);
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while(1) {
        const auto btnState = controller.state();
        gpio_put(PICO_DEFAULT_LED_PIN, btnState.at(btnToTest));
    }
}
