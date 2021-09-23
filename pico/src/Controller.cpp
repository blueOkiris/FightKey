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

GpioController::GpioController(const std::map<Button, uint> &btnMapping) :
        _btnMapping(btnMapping) {
    _initPins();
}

std::map<Button, bool> GpioController::state(void) const {
    std::map<Button, bool> curState;
    for(const auto &btnPinPair : _btnMapping) {
        curState.insert(std::make_pair(
            btnPinPair.first, gpio_get(btnPinPair.second)
        ));
    }
    return curState;
}

void GpioController::_initPins(void) const {
    for(const auto &btnPinPair : _btnMapping) {
        gpio_init(btnPinPair.second);
        gpio_set_dir(btnPinPair.second, false);
    }
}
