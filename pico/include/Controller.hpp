/*
 * Author: Dylan Turner
 * Description: Definition of a controller to read from
 */

#pragma once

#include <memory>
#include <map>
#include <Buttons.hpp>

namespace fightkey {
    struct Controller {
        virtual std::map<Button, bool> state(void) const = 0;
    };

    typedef std::shared_ptr<Controller> ControllerPtr;

    struct GpioController : public Controller {
        public:
            GpioController(const std::map<Button, uint> &btnMapping);
            std::map<Button, bool> state(void) const override;

        private:
            void _initPins(void) const;

            const std::map<Button, uint> _btnMapping;
    };
}
