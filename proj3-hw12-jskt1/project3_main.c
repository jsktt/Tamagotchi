// Starter code for Project 3. Good luck!

/* TI includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

/* Standard includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* HAL includes */
#include "HAL/LED.h"
#include "HAL/Timer.h"
#include "HAL/Button.h"
#include "HAL/Joystick.h"
#include <Application.h>
#include <HAL/HAL.h>



#define THREE_SECONDS       3000 //3seconds

#define PLAYPEN_LEFT  20
#define PLAYPEN_RIGHT 120


void initialize();
void sleep();

int main(void)
{
    initialize();

    HAL hal = HAL_construct();
    Application app = Application_construct();

    /* Graphics context. Works in the same as it did in the previous projects.
       You may choose to make a HAL struct and store the Graphics context in there instead.
       You may also choose to use the GFX struct from Guess th1e Color. */

    Joystick joystick = Joystick_construct();

    while (1) {
        sleep();


        Joystick_refresh(&joystick);
        Application_loop(&app, &hal, &joystick);
    }


}

Application Application_construct() {

    Application app;
    // Initialize local application state variables here!
    app.firstCall = true;
    app.gameScreenShown = false;
    app.age = -1;
    app.happinessLevel = 3;
    app.energyLevel = 5;
    app.state = TITLE_SCREEN;
    app.timer = SWTimer_construct(THREE_SECONDS);
    SWTimer_start(&app.timer);

    //creating age timer
    app.ageTimer = SWTimer_construct(THREE_SECONDS);
    SWTimer_start(&app.ageTimer);

    //energy timer
    app.energyTimer = SWTimer_construct(THREE_SECONDS);
    SWTimer_start(&app.energyTimer);

    //happy timer
    app.happinessTimer = SWTimer_construct(THREE_SECONDS);
    SWTimer_start(&app.happinessTimer);

    //tamagotchi pos & size
    app.tamagotchiX = 65;
    app.tamagotchiY = 90;
    app.prev_posx = 65;
    app.prev_posy = 90;
    app.tamagotchiR = 5;

    app.counter = 0;
    app.teen = 0;

    return app;
}

void Application_init(Application* app_p) {

    app_p->age = -1;
    app_p->energyLevel = 3;
    app_p->happinessLevel = 5;
    app_p->tamagotchiX = 65;
    app_p->tamagotchiX = 90;
    app_p->counter = 0;
    app_p->teen = 0;
    app_p->firstCall = true;
    app_p->state = TITLE_SCREEN;
    app_p->timer = SWTimer_construct(THREE_SECONDS);
    app_p->energyTimer = SWTimer_construct(THREE_SECONDS);
    app_p->happinessTimer = SWTimer_construct(THREE_SECONDS);
    app_p->tamagotchiR = 5;
    app_p->prev_posx = 65;
    app_p->prev_posy = 90;
    app_p->gameScreenShown = false;

}


void sleep() {
    // The Launchpad Green LED is used to signify the processor is in low-power mode.
    // From the human perspective, it should seem the processor is always asleep except for fractions of second here and there.

    TurnOn_LLG();
    // Enters the Low Power Mode 0 - the processor is asleep and only responds to interrupts
    PCM_gotoLPM0();
    TurnOff_LLG();

}

void Application_loop(Application* app_p, HAL* hal_p,Joystick *joystick_p) {
    // The first step is update the tapping status of all buttons
    buttons_t buttons = updateButtons();

    if(app_p->firstCall) {

        app_p->state = TITLE_SCREEN;
        Application_showTitleScreen(app_p, &hal_p->gfxContext);
        app_p->firstCall = false; // prevents this if-statement from running again

    }
    // Do not remove this statement. This is the non-bocking code for this project
    // Tapping the Joystick push button (JSB) toggles the BoosterPack Green LED
    if (buttons.BB1tapped)
        Toggle_BLG();

    // TODO: You probably want to define a state variable and an enum associated with it
    // You can then control the state of your application based on the hardware inputs
    // Refer to the Guess-the-color example.
        switch (app_p->state) {

            case TITLE_SCREEN:
                Application_handleTitleScreen(app_p, hal_p);
                break;
            case INSTRUCTION_SCREEN:
                if (buttons.BB1tapped)
                {
                    app_p->state = GAME_SCREEN;
                    Application_showGameScreen(app_p, &hal_p->gfxContext);
                    app_p->gameScreenShown = true;
                }
                break;
            case GAME_SCREEN:
                Application_handleGameScreen(app_p, hal_p, joystick_p);

                if (buttons.BB1tapped) {
                    app_p->energyLevel++;
                }
                if (app_p->energyLevel > 0) {
                    if (Joystick_isTappedToLeft(joystick_p)) {
                        app_p->tamagotchiX--;
                        app_p->counter++;
                        if (app_p->counter % 2 == 0) {
                            app_p->energyLevel--;
                            app_p->happinessLevel++;
                        }
                    }

                    if (Joystick_isTappedToRight(joystick_p)) {
                        app_p->tamagotchiX++;
                        app_p->counter++;
                        if (app_p->counter % 2 == 0) {
                            app_p->energyLevel--;
                            app_p->happinessLevel++;
                        }
                    }
                }
                break;

            case GAMEOVER_SCREEN:
                if (buttons.BB1tapped) {
                    Application_init(app_p);
                }
                break;
        }

}

// Initialization part is always device dependent and therefore does not change much with HAL
void initialize()
{

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);
    InitSystemTiming();

    initLEDs();
    initButtons();

    // TODO: Add other initializations here
}


void Application_handleTitleScreen(Application* app_p, HAL* hal_p) {

    if (SWTimer_expired(&app_p->timer))
    {
        app_p->state = INSTRUCTION_SCREEN;
        app_p->timer = SWTimer_construct(THREE_SECONDS);
        Application_showInstructionScreen(app_p, &hal_p->gfxContext);
    }

}

void Application_handleGameScreen(Application* app_p, HAL* hal_p, Joystick *joystick_p) {


    if (app_p->gameScreenShown)
    {

        //when three seconds expire, increment the age count.
        if (SWTimer_expired(&app_p->ageTimer)) {
            app_p->age++;
            SWTimer_start(&app_p->ageTimer);
        }

        // energy level
        if (SWTimer_expired(&app_p->energyTimer)) {
            app_p->energyLevel--;
            SWTimer_start(&app_p->energyTimer);
        }

        // happiness level
        if (SWTimer_expired(&app_p->happinessTimer)) {
            app_p->happinessLevel--;
            if (app_p->happinessLevel <= 0) {
                app_p->happinessLevel = 0;
            }
            SWTimer_start(&app_p->happinessTimer);
        }
        app_p->state = GAME_SCREEN;
        app_p->timer = SWTimer_construct(THREE_SECONDS);
        Application_updateGameScreen(app_p, &hal_p->gfxContext);
    }

    //life cycle

    if(app_p->energyLevel >= 3 && app_p->happinessLevel >= 4 && app_p->age >= 3) {

        app_p->tamagotchiR = 15;
        app_p->teen++;;

    }
    else if (app_p->energyLevel >= 2 && app_p->happinessLevel >= 2 && app_p->age >= 7 && app_p->teen != 0) {

        app_p->tamagotchiR = 20;
    }

    if (app_p->energyLevel <= 0 && app_p->happinessLevel <= 0) {

        app_p->state = GAMEOVER_SCREEN;
        Application_showGameOverScreen(app_p, &hal_p->gfxContext);
    }
}

void Application_showTitleScreen(Application* app_p, Graphics_Context* gfxContext) {

    //clear the screen from old text states
    GrClearDisplay(gfxContext);

    //text display
    Graphics_drawString(gfxContext, "ECE 2564", -1, 0, 0, true);
    Graphics_drawString(gfxContext, "Project 3", -1, 0, 20, true);
    Graphics_drawString(gfxContext, "Junsung Kim", -1, 0, 30, true);
    Graphics_drawString(gfxContext, "Low-Power", -1, 0, 50, true);
    Graphics_drawString(gfxContext, "Tamagotchi", -1, 0, 60, true);

    Graphics_drawString(gfxContext, "Your interrupt-", -1, 0, 80, true);
    Graphics_drawString(gfxContext, "driven virtual pet!", -1, 0, 90, true);

}

void Application_showInstructionScreen(Application* app_p, Graphics_Context* gfxContext) {

    //clear the screen from old text states
    GrClearDisplay(gfxContext);

    //text display
    Graphics_drawString(gfxContext, "Welcome to the ", -1, 0, 0, true);
    Graphics_drawString(gfxContext, "land of amazing", -1, 0, 20, true);
    Graphics_drawString(gfxContext, "TAMAGOTCHI!", -1, 0, 30, true);
    Graphics_drawString(gfxContext, "Watch your pet", -1, 0, 50, true);
    Graphics_drawString(gfxContext, "grow by feeding", -1, 0, 60, true);
    Graphics_drawString(gfxContext, "and playing with it", -1, 0, 70, true);
    Graphics_drawString(gfxContext, "how long can ", -1, 0, 80, true);
    Graphics_drawString(gfxContext, "you go?! ", -1, 0, 90, true);

    Graphics_drawString(gfxContext, "Press BB1 to advance ", -1, 0, 100, true);

}

void Application_showGameScreen(Application* app_p, Graphics_Context* gfxContext) {

    //clear the screen from old text states
    GrClearDisplay(gfxContext);

    //text display
    Graphics_drawString(gfxContext, "Age: ", -1, 0, 0, true);
    Graphics_drawString(gfxContext, "Energy: ", -1, 0, 20, true);
    Graphics_drawString(gfxContext, "Happiness: ", -1, 0, 40, true);

    //rectangle display
    Graphics_Rectangle rect = {10, 60, 120, 120};
    Graphics_drawRectangle(gfxContext, &rect);

    //pet display
    Graphics_setForegroundColor(gfxContext, GRAPHICS_COLOR_BLUE);
}

void Application_showGameOverScreen(Application* app_p, Graphics_Context* gfxContext) {

    //clear the screen from old text states
    GrClearDisplay(gfxContext);

    //text display
    Graphics_drawString(gfxContext, "Your Tamagotchi", -1, 20, 20, true);
    Graphics_drawString(gfxContext, "decided that the ", -1, 20,30, true);
    Graphics_drawString(gfxContext, "wilderness was", -1, 20, 40, true);
    Graphics_drawString(gfxContext, "better", -1, 20, 50, true);


    Graphics_drawString(gfxContext, "Age: ", -1, 20, 70, true);
    char ageStr[3];
    sprintf(ageStr, "%d", app_p->age);
    Graphics_drawString(gfxContext, (int8_t*)ageStr, -1, 60, 70, true);

    Graphics_drawString(gfxContext, "Play again? (BB1)", -1, 20, 90, true);

}

void Application_updateGameScreen(Application* app_p, Graphics_Context* gfxContext) {

    // Clear the age display area
    Graphics_drawString(gfxContext, "    ", -1, 50, 0, true);

    // Draw the updated age
    char ageStr[3];
    sprintf(ageStr, "%d", app_p->age);
    Graphics_drawString(gfxContext, (int8_t*)ageStr, -1, 50, 0, true);


    //Clear the energy display area
    Graphics_drawString(gfxContext, "       ", -1, 50, 20, true);

    char energyString[5];
    memset(energyString, 0, sizeof(energyString));
    int i;
    for (i = 0; i < app_p->energyLevel; i++) {
        strcat(energyString, "*");
    }

    Graphics_drawString(gfxContext, (int8_t*)energyString, -1, 50, 20, true);


    //Clear the happiness display area
    Graphics_drawString(gfxContext, "          ", -1, 60, 40, true);

    char happinessString[5];
    memset(happinessString, 0, sizeof(happinessString));
    int j;
    for (j = 0; j < app_p->happinessLevel; j++) {
        strcat(happinessString, "*");
    }

    Graphics_drawString(gfxContext, (int8_t*)happinessString, -1, 60, 40, true);

    Graphics_setForegroundColor(gfxContext, GRAPHICS_COLOR_WHITE);
    Graphics_fillCircle(gfxContext, app_p->prev_posx, app_p->prev_posy, app_p->tamagotchiR);

    // Draw the new circle
    Graphics_setForegroundColor(gfxContext, GRAPHICS_COLOR_BLUE);
    Graphics_fillCircle(gfxContext, app_p->tamagotchiX, app_p->tamagotchiY, app_p->tamagotchiR);

    // Update previous location
    app_p->prev_posx = app_p->tamagotchiX;
    app_p->prev_posy = app_p->tamagotchiY;

}


