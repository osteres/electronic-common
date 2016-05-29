//
// Created by Thibault PLET on 29/05/2016.
//

#ifndef COM_OSTERES_AUTOMATION_SAMPLE_SAMPLEAPPLICATION_H
#define COM_OSTERES_AUTOMATION_SAMPLE_SAMPLEAPPLICATION_H

#define ADDR_COUNTER 0x12

#include <Arduino.h>
#include <com/osteres/automation/Application.h>
#include <LiquidCrystal.h>
#include <com/osteres/automation/memory/Value.h>

using com::osteres::automation::Application;
using com::osteres::automation::memory::Value;

namespace com
{
    namespace osteres
    {
        namespace automation
        {
            namespace sample
            {
                class SampleApplication : Application
                {
                public:
                    /**
                     * Constructor
                     */
                    SampleApplication(LiquidCrystal &screen, unsigned int pinLight) {
                        this->screen = &screen;
                        this->pinLight = pinLight;

                        // Init counter
                        Value<unsigned int> c(ADDR_COUNTER, 0);
                        this->counter = &c;
                    }

                    /**
                     * Init application
                     */
                    void setup() {
                        // Enable screen light
                        pinMode(this->pinLight, OUTPUT);
                        digitalWrite(this->pinLight, HIGH);

                        // Init screen
                        this->screen->begin(16, 2);
                        this->screen->display();
                    }

                    /**
                     * Process application
                     */
                    void process() {
                        // Increment counter
                        this->counter->set(this->counter->get() + 1);

                        // Clear screen
                        this->screen->clear();

                        // Display counter
                        this->screen->setCursor(0, 0);
                        String output = "Counter:" + String(this->counter->get());
                        this->screen->write(output.c_str());

                        // Wait 1s
                        delay(1000);
                    }

                protected:
                    /**
                     * Screen
                     */
                    LiquidCrystal * screen;

                    /**
                     * Counter
                     */
                    Value<unsigned int> * counter;

                    /**
                     * Screen pin light
                     */
                    unsigned int pinLight;
                };
            }
        }
    }
}


#endif //COM_OSTERES_AUTOMATION_SAMPLE_SAMPLEAPPLICATION_H
