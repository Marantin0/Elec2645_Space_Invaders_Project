/**
@file main.h
@brief Header file containing functions prototypes, defines and global variables.
@author Martin T. Georgiev
@date   May 2016
*/

#ifndef MAIN_H
#define MAIN_H

#define PI 3.14159265359

#include "mbed.h"

/**  
@namespace myled
@brief GPIO output for status LED
*/
/** LCD screen inputs and outputs
*/
//         VCC,    SCE,   RST,   D/C,   MOSI,  SCLK,   LED
N5110 lcd (PTE26 , PTA0 , PTC4 , PTD0 , PTD2 , PTD1 , PTC3);

// change this to alter tolerance of joystick direction
#define DIRECTION_TOLERANCE 0.05

// connections for joystick
/** 
Joystick button
*/
DigitalIn button(PTB11);
/**
Joystick X directions
*/
AnalogIn xPot(PTB3);
/**
Joystick Y value
*/
AnalogIn yPot(PTB2);

// timer to regularly read the joystick
/**
Ticker for Joystick
*/
Ticker pollJoystick;
/**
interrupts
*/
Ticker ticker1,ticker2;

/**
 create enumerated type (0,1,2,3 etc. for direction)
 could be extended for diagonals etc.
 */
enum DirectionName {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTRE,
    UNKNOWN
};

/**
structfor Joystick
*/
typedef struct JoyStick Joystick;
struct JoyStick {
    float x;    // current x value
    float x0;   // 'centred' x value
    float y;    // current y value
    float y0;   // 'centred' y value
    int button; // button state (assume pull-down used, so 1 = pressed, 0 = unpressed)
    DirectionName direction;  // current direction
};


/**
Ticker interrupts
*/
void timer1_isr();
void timer2_isr();

//-------------------------------Variables-------------------------//

/** 
variable used in detecting whether game has finished of not
*//
bool gameState = false; //This variable checks whether the game is on.
/**
variable used in checking whether any enemy sprites have reached the player
*/
bool enemyWin = false;  //This variable is used to check if any sprites have reached the player

/**
array used to determine every aliens on or off state
*/
bool enemy [15];        //this array checks the state of each individual enemy sprite;
/**
varuable used to count how any enemy sprites are still on
*/
int enemyCounter = 16;  //count the number of enemies still alive
/** variable used to determine whether or not projectile has hit enemy sprite of top of screen
*/
bool bullet = false;
/**
variable holding screen parameters
*/
int screenW = 83, screenH = 47;
/**
variables containing values of all enemy X and Y coordinates
*/
int enemyX1 = 2 , enemyY1 = 4 , enemyX2 = 10 , enemyY2 = 4, enemyX3 = 18 , enemyY3 = 4,
    enemyX4 = 26 , enemyY4 = 4 , enemyX5 = 34 , enemyY5 = 4 ,enemyX6 = 42 , enemyY6 = 4, 
    enemyX7 = 50 , enemyY7 = 4 , enemyX8 = 58 , enemyY8 = 4 , enemyX9 = 2 , enemyY9 = 11 ,
    enemyX10 = 10 , enemyY10 = 11 , enemyX11 = 18 , enemyY11 = 11 , enemyX12 = 26 , enemyY12 = 11 , 
    enemyX13 = 34 , enemyY13 = 11 , enemyX14 = 42 , enemyY14 = 11 , enemyX15 = 50 , enemyY15 = 11,
    enemyX16 = 58 , enemyY16 = 11 ;
/**
Varibles used to move enemy Sprites down Rows and also declare their on/off state
*/    
int enemyRowCounter = 0, enemyNumber = 0;   //this variable are used to determine which row the aliens 
/**
Variables containing player X and Y coordinates
*/
int  playerX = 42 , playerY = 46 ;
/** Value of ticker2 time and Delay used in startUp function;
*/
float t = 0.05, Delay=0;
/**
Variables containing Projectile X and Y coordinates
*/
int bulletX = playerX, bulletY = playerY-6;

//-----------------------------------Functions--------------------------------//
/**
funtion played on device start up which displays module code and author
*/
void startUp();
/**
Funtion containing project Menu
*/
void Menu();
/**
Funtion containing all game functions
*/
void Game();
/**
Time funtion used for convenience instead of if statements
*/
void timeDelay();
 /**
 funtion that turns all enemy sprites on before starting game
 */
void alienStateInitialise();
/** 
Funtion that draws enemy sprites and determined whether certain alien is on/off
*/
void alien(int x, int y,bool state);
/**
funtion containing all enemy sprites
*/
void groupAlien(int x,int y);
/**
Funtion responsible for the movement of enemy sprites
*/
void moveAliens();
/**
funtion that draws player sprite
*/
void player(int x, int y);
/** 
function responsible for player sprite movement
*/
int playerMove();
/**
Function that draws the projetile Sprite 
*/
void projectile(int x, int y);
/**
Funtion responsible for firing projectile
*/
void fireProjectile();
/**
Function which detect whether projectile has collided with top of screen or enemy srites that are still on
*/
void collision();
/**
funtion which detects whether any enemy has reached player
*/
void enemyDetection();

/**
Funtion to calibrate neutral position of Joystick
*/
void calibrateJoystick();
/**
Funtion updating the joystick values
*/
void updateJoystick();