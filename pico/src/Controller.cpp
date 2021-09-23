/*
 * Author: Dylan Turner
 * Description: Implementation of a controller to read from
 */

#include <map>
#include <memory>
#include <stdio.h>
#include <pico/stdlib.h>
#include <Buttons.hpp>
#include <Controller.hpp>

using namespace fightkey;

GpioController::ButtonController(const std::map<Button, uint> &btnMapping) :
        _btnMapping(btnMapping) {
    _initPins();
}

std::map<Button, bool> GpioController::state(void) const {
    std::map<Button, bool> curState;
    for(const auto &btnPinPair : _btnMapping) {
        curState.insert(std::make_pair(
            btnPinPair.second, gpio_get(btnPinPair.first)
        );
    }
    return curState;
}

void GpioController::_initPins(void) const {
    for(const auto &btnPinPair : _btnMapping) {
        gpio_init(btnPinPair.second);
        gpio_set_dir(btnPair.second, false);
    }
}
