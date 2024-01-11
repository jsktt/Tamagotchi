/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_


#include <HAL/HAL.h>

struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    //UART_Baudrate baudChoice;
    bool firstCall;
    bool gameScreenShown;
    GameState state;
    SWTimer timer; //timer for when screen must disappear
    SWTimer ageTimer; //for age
    SWTimer happinessTimer; //for happiness level
    SWTimer energyTimer; // energy level
    Cursor cursor;
    int age;
    int happinessLevel;
    int energyLevel;

    //tamagotchi pos
    int tamagotchiX;
    int tamagotchiY;
    int tamagotchiR;
    int prev_posx;
    int prev_posy;
    int counter;
    int teen;


};
typedef struct _Application Application;



// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal, Joystick* joystick);

// Called only a single time - inside of Application_loop(), but only when
// the firstCall variable is true.
void Application_firstTimeSetup(Application* app_p, HAL* hal_p);

// Called whenever the UART module needs to be updated
void Application_updateCommunications(Application* app, HAL* hal);

// Interprets an incoming character and echoes back to terminal what kind of
// character was received (number, letter, or other)
char Application_interpretIncomingChar(char, Graphics_Context* gfxContext);

// Generic circular increment function
uint32_t CircularIncrement(uint32_t value, uint32_t maximum);

//Handle callback functions
void Application_handleTitleScreen(Application* app, HAL* hal);
void Application_handleInstructionScreen(Application* app, HAL* hal);
void Application_handleGameScreen(Application* app, HAL* hal, Joystick* joystick);

//Update functions
void Application_updateGameScreen(Application* app_p, Graphics_Context* gfxContext);


//helper functions
void Application_showTitleScreen(Application* app_p, Graphics_Context* gfxContext);
void Application_showInstructionScreen(Application* app_p, Graphics_Context* gfxContext);
void Application_showGameScreen(Application* app_p, Graphics_Context* gfxContext);
void Application_showGameOverScreen(Application* app_p, Graphics_Context* gfxContext);


#endif /* APPLICATION_H_ */
