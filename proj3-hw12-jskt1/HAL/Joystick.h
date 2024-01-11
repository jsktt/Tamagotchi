/*
 * joystick.h
 *
 *  Created on: Mar 30, 2023
 *      Author: Junsung
 */

#ifndef HAL_JOYSTICK_H_
#define HAL_JOYSTICK_H_


#include <ti/devices/msp432p4xx/driverlib/driverlib.h>


struct _Joystick
{

    uint_fast16_t x;
    uint_fast16_t y;

    bool isTappedToLeft;
    bool isTappedToRight;
    bool isTappedUp;
    bool isTappedDown;
    bool isPressed;
    int pushState;


};
typedef struct _Joystick Joystick;



/** Given a Joystick, determines if the joystick is currently pressed left */
bool Joystick_isPressedToLeft(Joystick* joystick_p);

/** Given a Joystick, determines if it was "tapped" to left - went from middle to left */
bool Joystick_isTappedToLeft(Joystick* joystick_p);

/** Given a Joystick, determines if it was "tapped" to right - went from middle to right */
bool Joystick_isTappedToRight(Joystick* joystick_p);

/** Refreshes this Joystick so the Joystick FSM now has new outputs to interpret */
void Joystick_refresh(Joystick* joystick_p);

/** Given a Joystick, determines if it was "tapped" up - went from middle to up */
bool Joystick_isTappedUp(Joystick* joystick_p);

bool Joystick_isTappedDown(Joystick* joystick_p);



Joystick Joystick_construct();



#endif /* HAL_JOYSTICK_H_ */
