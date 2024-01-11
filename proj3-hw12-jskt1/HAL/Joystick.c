/*
 * joystick.c
 *
 *  Created on: Mar 30, 2023
 *      Author: Junsung
 */

#include <HAL/Joystick.h>
#define LEFT_THRESHOLD 3000
#define RIGHT_THRESHOLD 12000
#define DOWN_THRESHOLD 3000
#define UP_THRESHOLD 14000

enum _JoystickDebounceState {

    LEFT,
    NOT_LEFT,
    RIGHT,
    NOT_RIGHT,
    UP,
    NOT_UP,
    DOWN,
    NOT_DOWN
};
typedef enum _JoystickDebounceState JoystickDebounceState;

static volatile bool joystickX_modified;

void ADC14_IRQHandler() {
    if (ADC14_getEnabledInterruptStatus() & ADC_INT0) {
        joystickX_modified = true;
    }

    ADC14_clearInterruptFlag(ADC_INT0);
}

// Initializing the ADC which resides on SoC
void initADC() {
    ADC14_enableModule();

    ADC14_initModule(ADC_CLOCKSOURCE_SYSOSC,
                     ADC_PREDIVIDER_64,
                     ADC_DIVIDER_8,
                      0
                     );

    // This configures the ADC to store output results
    // in ADC_MEM0 for joystick X.
    // Todo: if we want to add joystick Y, then, we have to use more memory locations
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);

    ADC14_setSampleHoldTime(ADC_PULSE_WIDTH_192, ADC_PULSE_WIDTH_192);

    // This configures the ADC in manual conversion mode
    // Software will start each conversion.
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

}


void startADC() {
   // Starts the ADC with the first conversion
   // in repeat-mode, subsequent conversions run automatically
   ADC14_enableConversion();
   ADC14_toggleConversionTrigger();
}


// Interfacing the Joystick with ADC (making the proper connections in software)
void initJoyStick() {

    // This configures ADC_MEM0 to store the result from
    // input channel A15 (Joystick X), in non-differential input mode
    // (non-differential means: only a single input pin)
    // The reference for Vref- and Vref+ are VSS and VCC respectively
    ADC14_configureConversionMemory(ADC_MEM0,
                                  ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                  ADC_INPUT_A15,                 // joystick X
                                  ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A15 is multiplexed on GPIO port P6 pin PIN0
    // TODO: which one of GPIO_PRIMARY_MODULE_FUNCTION, or
    //                    GPIO_SECONDARY_MODULE_FUNCTION, or
    //                    GPIO_TERTIARY_MODULE_FUNCTION
    // should be used in place of 0 as the last argument?
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                               GPIO_PIN0,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    ADC14_configureConversionMemory(ADC_MEM1,
                                    ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    ADC_INPUT_A9,                 // joystick Y
                                    ADC_NONDIFFERENTIAL_INPUTS);

    // This selects the GPIO as analog input
    // A15 is multiplexed on GPIO port P6 pin PIN0
    // TODO: which one of GPIO_PRIMARY_MODULE_FUNCTION, or
    //                    GPIO_SECONDARY_MODULE_FUNCTION, or
    //                    GPIO_TERTIARY_MODULE_FUNCTION
    // should be used in place of 0 as the last argument?
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4,
                                               GPIO_PIN4,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    //set up interrupts for the ADC
    ADC14_clearInterruptFlag(ADC_INT0);
    ADC14_enableInterrupt(ADC_INT0);

    Interrupt_enableInterrupt(INT_ADC14);

}

/**
 * Constructs a Joystick
 * Initializes the output FSMs.
 *
 * @return a constructed Joystick with debouncing and output FSMs initialized
 */


Joystick Joystick_construct()
{
    // The Joystick object which will be returned at the end of construction
    Joystick Joystick;

    initADC();
    initJoyStick();
    startADC();


    // Initialize all buffered outputs of the Joystick
    //Joystick.pushState = RELEASED;
    //    Joystick.isTapped = false;

    // Return the constructed Joystick object to the user
    return Joystick;
}


void Joystick_refresh(Joystick* joystick_p)
{
    if (joystickX_modified) {
        joystick_p->x = ADC14_getResult(ADC_MEM0);

        joystickX_modified = false;
    }
    //joystick_p->y = ADC14_getResult(ADC_MEM1);

    static JoystickDebounceState rightState = NOT_RIGHT;
    joystick_p->isTappedToRight = false;

    switch(rightState) {

    case NOT_RIGHT:

        if (joystick_p->x < RIGHT_THRESHOLD) {
            rightState = RIGHT;
            joystick_p->isTappedToRight = true;
        }
        break;
    case RIGHT:
        if (joystick_p->x > RIGHT_THRESHOLD) {
            rightState = NOT_RIGHT;
        }
        break;
    }


    static JoystickDebounceState leftState = NOT_LEFT;
    joystick_p->isTappedToLeft = false;

    switch(leftState) {

    case NOT_LEFT:

        if (joystick_p->x < LEFT_THRESHOLD) {
            leftState = LEFT;
            joystick_p->isTappedToLeft = true;
        }
        break;
    case LEFT:
        if (joystick_p->x > LEFT_THRESHOLD) {
            leftState = NOT_LEFT;
        }
        break;
    }

}

/** Given a Joystick, determines if the joystick is currently pressed left */
bool Joystick_isPressedToLeft(Joystick* joystick_p){

    return (joystick_p->x < LEFT_THRESHOLD);
}

bool Joystick_isTappedToLeft(Joystick* joystick_p) {

    return (joystick_p->isTappedToLeft);
}

bool Joystick_isTappedToRight(Joystick* joystick_p) {

    return (joystick_p->isTappedToRight);
}

bool Joystick_isTappedUp(Joystick* joystick_p){

    return (joystick_p->isTappedUp);
}

bool Joystick_isTappedDown(Joystick* joystick_p){

    return (joystick_p->isTappedDown);
}

