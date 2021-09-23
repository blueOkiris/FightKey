/*
 * Author: Dylan Turner
 * Description: A place to store the button enum Controller & HID use
 */

#pragma once

namespace fightkey {
    enum class Button {
        Up, Down, Left, Right, Start, Select,
        LightPunch, MediumPunch, HeavyPunch, AllPunch,
        LightKick, MediumKick, HeavyKick, AllKick,
        Escape
    };
}
