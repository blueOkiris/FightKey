/*
 * Author: Dylan Turner
 * Description: Entry point for Keyboard system
 */

#include <bsp/board.h>
#include <map>
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
    { Button::LightPunch,   HID_KEY_J },
    { Button::MediumPunch,  HID_KEY_K },
    { Button::HeavyPunch,   HID_KEY_L },
    { Button::AllPunch,     HID_KEY_BRACKET_LEFT },
    { Button::LightKick,    HID_KEY_U },
    { Button::MediumKick,   HID_KEY_I },
    { Button::HeavyKick,    HID_KEY_O },
    { Button::AllKick,      HID_KEY_SEMICOLON }
};

int main(void) {
    const GpioController controller(g_btnMapping);
    HidKeyboard keyboard(g_keyMapping);

    while(1) {
        keyboard.update();
        
        const auto btnState = controller.state();
        for(const auto &btnStatePair : btnState) {
            if(btnStatePair.second) {
                keyboard.pressKey(btnStatePair.first);
                keyboard.releaseKey();
            }
        }
    }
    return 0;
}
