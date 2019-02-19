#include "mbed.h"
#include "N5110.h"
DigitalOut led(LED_RED);
//         VCC,    SCE,   RST,   D/C,   MOSI,  SCLK,   LED
N5110 lcd (PTE26 , PTA0 , PTC4 , PTD0 , PTD2 , PTD1 , PTC3);

// change this to alter tolerance of joystick direction
#define DIRECTION_TOLERANCE 0.05

// connections for joystick
DigitalIn button(PTB11);
AnalogIn xPot(PTB3);
AnalogIn yPot(PTB2);

// timer to regularly read the joystick
Ticker pollJoystick;
Ticker ticker1,ticker2;
// create enumerated type (0,1,2,3 etc. for direction)
// could be extended for diagonals etc.
enum DirectionName {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTRE,
    UNKNOWN
};

// structfor Joystick
typedef struct JoyStick Joystick;
struct JoyStick {
    float x;    // current x value
    float x0;   // 'centred' x value
    float y;    // current y value
    float y0;   // 'centred' y value
    int button; // button state (assume pull-down used, so 1 = pressed, 0 = unpressed)
    DirectionName direction;  // current direction
};

// create struct variable
Joystick joystick;
 volatile int g_timer1_flag = 0;
 volatile int g_timer2_flag = 0;
int printFlag = 0;

// function prototypes
void calibrateJoystick();
void updateJoystick();
// function prototypes
void timer1_isr();
void timer2_isr();

bool gameState = false; //This variable checks whether the game is on.
bool enemyWin = false;  //This variable is used to check if any sprites have reached the player
    
bool enemy [15];        //this array checks the state of each individual enemy sprite;
int enemyCounter = 16;  //count the number of enemies still alive
bool bullet = false;

int screenW = 83, screenH = 47;
int enemyX1 = 2 , enemyY1 = 4 , enemyX2 = 10 , enemyY2 = 4, enemyX3 = 18 , enemyY3 = 4,
    enemyX4 = 26 , enemyY4 = 4 , enemyX5 = 34 , enemyY5 = 4 ,enemyX6 = 42 , enemyY6 = 4, 
    enemyX7 = 50 , enemyY7 = 4 , enemyX8 = 58 , enemyY8 = 4 , enemyX9 = 2 , enemyY9 = 11 ,
    enemyX10 = 10 , enemyY10 = 11 , enemyX11 = 18 , enemyY11 = 11 , enemyX12 = 26 , enemyY12 = 11 , 
    enemyX13 = 34 , enemyY13 = 11 , enemyX14 = 42 , enemyY14 = 11 , enemyX15 = 50 , enemyY15 = 11,
    enemyX16 = 58 , enemyY16 = 11 ;
    
int enemyRowCounter = 0, enemyNumber = 0;   //this variable are used to determine which row the aliens 
int  playerX = 42 , playerY = 46 ;
float t = 0.05, Delay=0;
int bulletX = playerX, bulletY = playerY-6;

void startUp();
void Menu();
void Game();
void timeDelay();
 
void alienStateInitialise();
void alien(int x, int y,bool state);
void groupAlien(int x,int y);
void moveAliens();

void player(int x, int y);
int playerMove();
void projectile(int x, int y);
void fireProjectile();
void collision();
void enemyDetection();

int main()
{
   
    calibrateJoystick();  // get centred values of joystick
    pollJoystick.attach(&updateJoystick,1.0/10);  // read joystick 20 times per second 
    // set-up the ticker so that the ISR it is called every 1 seconds
    ticker1.attach(&timer1_isr,2);
    ticker2.attach(&timer2_isr,t);
    lcd.init();
    startUp();
    
    
    while(1){
       timeDelay();
           
           if(gameState == true){
               while((gameState == true) ) {
                 Game();
                }
        } 
    }

    
    
}
void timeDelay(){
    if (g_timer1_flag) {  
            g_timer1_flag = 0;
            Delay++;
            
            
        }
        if (Delay >=2){
                gameState=true;
                enemyCounter = 16;
                alienStateInitialise();
                Delay = 0;
                enemyWin = false;
            }
    }

void startUp(){
    lcd.printString("Elec2645",10,0);
    lcd.printString("Project",10,1);
    lcd.printString("By",10,2);
    lcd.printString("Martin",10,3);
    lcd.printString("Georgiev",10,4);
}
    
void Menu(){
    
}
void Game(){  //Function containing all parameter and funtion of actual game
    
    player(playerX,playerY);
    playerMove();
    moveAliens();
    enemyDetection();
    lcd.refresh();
    lcd.clear();
           
    if (enemyCounter <= 0){                 //if all 16 enemy sprites are off, display message
        lcd.printString("Victory!",15,2);
        
            if (g_timer1_flag) {  
                        g_timer1_flag = 0;
                    }
            gameState = false;
            lcd.refresh();
            }
            
             else if (enemyWin == true){        //if enemy Sprites have reached player, display message
                lcd.printString("Game Over",10,2);
                    
            if (g_timer1_flag) {  
                        g_timer1_flag = 0;
                    }
                    
            gameState = false;
            lcd.refresh();
        }
    
    }


void projectile(int x, int y){      //This function draws the projectile sprite
    
    lcd.setPixel(x,y);
    lcd.setPixel(x,y-1);
    lcd.setPixel(x,y-2);
    lcd.setPixel(x,y-3);
    lcd.setPixel(x,y-4);
    }
    
void collision(){       // funtion checking if projectile has hit any enemies or the top of the screen and if so turning them off
        
        if(bullet == true){         //if button is pressed, fire projectile
          projectile(bulletX,bulletY);
            bulletY--;
            }
            
        if( bulletY <= (screenH/screenH) -1 ) { //if projectile has reached top of screen, allow player to shoop another projectile
                bullet = false;
            } 
            
        else if((((bulletX == enemyX1)||(bulletX == enemyX1+1)||    //turn off enemy sprites that have been hit by projectile
                (bulletX == enemyX1+2)||(bulletX == enemyX1-1)||
                (bulletX == enemyX1-1))&&(bulletY == enemyY1))&&(enemy[0] == true)){
           
                        enemy[0] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX2)||(bulletX == enemyX2+1)||
                (bulletX == enemyX2+2)||(bulletX == enemyX2-1)||
                (bulletX == enemyX2-1))&&(bulletY == enemyY2))&&(enemy[1] == true)){
           
                        enemy[1] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX3)||(bulletX == enemyX3+1)||
                (bulletX == enemyX3+2)||(bulletX == enemyX3-1)||
                (bulletX == enemyX3-1))&&(bulletY == enemyY3))&&(enemy[2] == true)){
           
                        enemy[2] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX4)||(bulletX == enemyX4+1)||
                (bulletX == enemyX4+2)||(bulletX == enemyX4-1)||
                (bulletX == enemyX4-1))&&(bulletY == enemyY4))&&(enemy[3] == true)){
           
                        enemy[3] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX5)||(bulletX == enemyX5+1)||
                (bulletX == enemyX5+2)||(bulletX == enemyX5-1)||
                (bulletX == enemyX5-1))&&(bulletY == enemyY5))&&(enemy[4] == true)){
           
                        enemy[4] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX6)||(bulletX == enemyX6+1)||
                (bulletX == enemyX6+2)||(bulletX == enemyX6-1)||
                (bulletX == enemyX6-1))&&(bulletY == enemyY6))&&(enemy[5] == true)){
           
                        enemy[5] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX7)||(bulletX == enemyX7+1)||
                (bulletX == enemyX7+2)||(bulletX == enemyX7-1)||
                (bulletX == enemyX7-1))&&(bulletY == enemyY7))&&(enemy[6] == true)){
           
                        enemy[6] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX8)||(bulletX == enemyX8+1)||
                (bulletX == enemyX8+2)||(bulletX == enemyX8-1)||
                (bulletX == enemyX8-1))&&(bulletY == enemyY8))&&(enemy[7] == true)){
           
                        enemy[7] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX9)||(bulletX == enemyX9+1)||
                (bulletX == enemyX9+2)||(bulletX == enemyX9-1)||
                (bulletX == enemyX9-1))&&(bulletY == enemyY9))&&(enemy[8] == true)){
           
                        enemy[8] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX10)||(bulletX == enemyX10+1)||
                (bulletX == enemyX10+2)||(bulletX == enemyX10-1)||
                (bulletX == enemyX10-1))&&(bulletY == enemyY10))&&(enemy[9] == true)){
           
                        enemy[9] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX11)||(bulletX == enemyX11+1)||
                (bulletX == enemyX11+2)||(bulletX == enemyX11-1)||
                (bulletX == enemyX11-1))&&(bulletY == enemyY11))&&(enemy[10] == true)){
           
                        enemy[10] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX12)||(bulletX == enemyX12+1)||
                (bulletX == enemyX12+2)||(bulletX == enemyX12-1)||
                (bulletX == enemyX12-1))&&(bulletY == enemyY12))&&(enemy[11] == true)){
           
                        enemy[11] = false;
                        bullet = false;
                        enemyCounter--;
                   }
        else if((((bulletX == enemyX13)||(bulletX == enemyX13+1)||
                (bulletX == enemyX13+2)||(bulletX == enemyX13-1)||
                (bulletX == enemyX13-1))&&(bulletY == enemyY13))&&(enemy[12] == true)){
           
                        enemy[12] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX14)||(bulletX == enemyX14+1)||
                (bulletX == enemyX14+2)||(bulletX == enemyX14-1)||
                (bulletX == enemyX14-1))&&(bulletY == enemyY14))&&(enemy[13] == true)){
           
                        enemy[13] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX15)||(bulletX == enemyX15+1)||
                (bulletX == enemyX15+2)||(bulletX == enemyX15-1)||
                (bulletX == enemyX15-1))&&(bulletY == enemyY15))&&(enemy[14] == true)){
           
                        enemy[14] = false;
                        bullet = false;
                        enemyCounter--;
                   }
                   
        else if((((bulletX == enemyX16)||(bulletX == enemyX16+1)||
                (bulletX == enemyX16+2)||(bulletX == enemyX16-1)||
                (bulletX == enemyX16-1))&&(bulletY == enemyY16))&&(enemy[15] == true)){
           
                        enemy[15] = false;
                        bullet = false;
                        enemyCounter--;
                   }     
                
 }
 
void alienStateInitialise(){     //This function turns all aliens on before starting the game
    for(int y=4 ; y<=25 ; y+=7){
        for( int x=2; x<=58 ; x+=8){
            
            enemy[enemyNumber] = true;
            alien(x, y, enemy[enemyNumber]);
            enemyNumber++;
        }
    }
}

void alien(int x, int y, bool state){   //This Function creates the sprite for the aliens
    
        if (state == true){
            lcd.setPixel(x,y);
            lcd.setPixel(x-1,y);
            lcd.setPixel(x-2,y);
            lcd.setPixel(x+1,y);
            lcd.setPixel(x+2,y);
            lcd.setPixel(x,y+1);
            lcd.setPixel(x-2,y+1);
            lcd.setPixel(x+2,y+1);
            lcd.setPixel(x+2,y+2);
            lcd.setPixel(x+1,y+2);
            lcd.setPixel(x-1,y+2);
            lcd.setPixel(x-2,y+2);
            lcd.setPixel(x,y+3);
            lcd.setPixel(x-1,y+3);
            lcd.setPixel(x+1,y+3);
            lcd.setPixel(x-2,y+4);
            lcd.setPixel(x+2,y+4);
         
        }
        
}

void groupAlien(){  //this funtion groups all the aliens together for convenience
    
    
    alien(enemyX1,enemyY1,enemy[0]);
    alien(enemyX2,enemyY2,enemy[1]);
    alien(enemyX3,enemyY3,enemy[2]);
    alien(enemyX4,enemyY4,enemy[3]);
    alien(enemyX5,enemyY5,enemy[4]);
    alien(enemyX6,enemyY6,enemy[5]);
    alien(enemyX7,enemyY7,enemy[6]);
    alien(enemyX8,enemyY8,enemy[7]);
    alien(enemyX9,enemyY9,enemy[8]);
    alien(enemyX10,enemyY10,enemy[9]);
    alien(enemyX11,enemyY11,enemy[10]);
    alien(enemyX12,enemyY12,enemy[11]);
    alien(enemyX13,enemyY13,enemy[12]);
    alien(enemyX14,enemyY14,enemy[13]);
    alien(enemyX15,enemyY15,enemy[14]);
    alien(enemyX16,enemyY16,enemy[15]);
    
}
    
void moveAliens(){   //this function checks the whereabouts of the aliens and depenting on their vertical location on the screen
                    //either moves them left or right every 't' value seconds
        
        if (g_timer2_flag) {  
                    g_timer2_flag = 0;
                
                    
                if((enemyRowCounter == 0)||(enemyRowCounter == 2 )||            //if Variable is equal to any of these value, the enemy X value increases
                    (enemyRowCounter == 4)||(enemyRowCounter == 6 )||           //and sprites move to the right
                    (enemyRowCounter == 8)||(enemyRowCounter == 10 )||
                    (enemyRowCounter == 12)||(enemyRowCounter == 14 )||
                    (enemyRowCounter == 16)||(enemyRowCounter == 18)) {
                    
                               enemyX1++;
                               enemyX2++;
                               enemyX3++;
                               enemyX4++;
                               enemyX5++;
                               enemyX6++;
                               enemyX7++;
                               enemyX8++;
                               enemyX9++;
                               enemyX10++;
                               enemyX11++;
                               enemyX12++;
                               enemyX13++;
                               enemyX14++;
                               enemyX15++;
                               enemyX16++;
                   
                   
                    
                                    if(enemyX8 >= screenW - 3) {        //if  variable reaches the end of screen, the sprites go down 2 rows
                                       
                                       enemyY1+=2;
                                       enemyY2+=2;
                                       enemyY3+=2;
                                       enemyY4+=2;
                                       enemyY5+=2;
                                       enemyY6+=2;
                                       enemyY7+=2;
                                       enemyY8+=2;
                                       enemyY9+=2;
                                       enemyY10+=2;
                                       enemyY11+=2;
                                       enemyY12+=2;
                                       enemyY13+=2;
                                       enemyY14+=2;
                                       enemyY15+=2;
                                       enemyY16+=2;
                                       
                                       t=t/3;
                                       
                                       enemyRowCounter++;
                                       }
                    
                    } 
                    
                else if((enemyRowCounter == 1)||(enemyRowCounter == 3)||        //if variable matches there, sprites move left
                        (enemyRowCounter == 5)||(enemyRowCounter == 7)||
                        (enemyRowCounter == 9)||(enemyRowCounter == 11)||
                        (enemyRowCounter == 13)||(enemyRowCounter == 15)||
                        (enemyRowCounter == 17)||(enemyRowCounter == 19)) {
                    
                               enemyX1--;
                               enemyX2--;
                               enemyX3--;
                               enemyX4--;
                               enemyX5--;
                               enemyX6--;
                               enemyX7--;
                               enemyX8--;
                               enemyX9--;
                               enemyX10--;
                               enemyX11--;
                               enemyX12--;
                               enemyX13--;
                               enemyX14--;
                               enemyX15--;
                               enemyX16--;
                    
                    if (enemyX1 <= (screenW/screenW)+1) {       //sprites move down 2 rows
                       
                       enemyY1+=2;
                       enemyY2+=2;
                       enemyY3+=2;
                       enemyY4+=2;
                       enemyY5+=2;
                       enemyY6+=2;
                       enemyY7+=2;
                       enemyY8+=2;
                       enemyY9+=2;
                       enemyY10+=2;
                       enemyY11+=2;
                       enemyY12+=2;
                       enemyY13+=2;
                       enemyY14+=2;
                       enemyY15+=2;
                       enemyY16+=2;
                       
                       
                       enemyRowCounter++;
                       t=t/3;
                       }
                }
          }  
        groupAlien();  
        collision();  
}    
    
void enemyDetection(){                  //checks bit on the 44th row , and if enemy touches this line, aliens win
    for( int x =0; x<=83; x++){
        if(lcd.getPixel(x,44)){
            enemyWin = true;;
        }
    
    }
  }  
void player(int x, int y){              //draws the sprite for the player
        
    lcd.setPixel(x,y);
    lcd.setPixel(x-1,y);
    lcd.setPixel(x+1,y);
    lcd.setPixel(x,y-1);
    
    }
    
int playerMove(){       //checks the controller and if controller is tilted left,right, up or is pressed, either moves the player left/right or shoots the projectile
        
        if (printFlag) {  // if flag set, clear flag and print joystick values to serial port
            printFlag = 0;         
            
            // check joystick direction
            if (joystick.direction == LEFT)
               if(playerX>=0){
                     playerX-=3;
                     }
                else playerX=0;
             
            if (joystick.direction == RIGHT)   
                if(playerX<=83){
                     playerX+=3;
                     }
                else playerX=83;
                
             if (joystick.direction == CENTRE)   
                playerX=playerX;
                playerY=playerY;    
             
             if(button == 1){       //if button is pressed, fire projectile
                        
                    if ( bullet == false){
                            
                            bulletX=playerX;
                            bulletY=playerY-4;
                            bullet = true;
                            
                    }
                        
            }
        }
            
}


// read default positions of the joystick to calibrate later readings
void calibrateJoystick()
{
    button.mode(PullDown);
    // must not move during calibration
    joystick.x0 = xPot;  // initial positions in the range 0.0 to 1.0 (0.5 if centred exactly)
    joystick.y0 = yPot;
}

void updateJoystick()
{
    // read current joystick values relative to calibrated values (in range -0.5 to 0.5, 0.0 is centred)
    joystick.x = xPot - joystick.x0;
    joystick.y = yPot - joystick.y0;
    // read button state
    joystick.button = button;

    // calculate direction depending on x,y values
    // tolerance allows a little lee-way in case joystick not exactly in the stated direction
    if ( fabs(joystick.y) < DIRECTION_TOLERANCE && fabs(joystick.x) < DIRECTION_TOLERANCE) {
        joystick.direction = CENTRE;
    } else if ( joystick.y > DIRECTION_TOLERANCE && fabs(joystick.x) < DIRECTION_TOLERANCE) {
        joystick.direction = LEFT;
    } else if ( joystick.y < DIRECTION_TOLERANCE && fabs(joystick.x) < DIRECTION_TOLERANCE) {
        joystick.direction = RIGHT;
    } else if ( joystick.x > DIRECTION_TOLERANCE && fabs(joystick.y) < DIRECTION_TOLERANCE) {
        joystick.direction = DOWN;
    } else if ( joystick.x < DIRECTION_TOLERANCE && fabs(joystick.y) < DIRECTION_TOLERANCE) {
        joystick.direction = UP;
    } else {
        joystick.direction = UNKNOWN;
    }

    // set flag for printing
    printFlag = 1;
}
void timer1_isr(){

    g_timer1_flag = 1;   // set flag in ISR
}
void timer2_isr(){

    g_timer2_flag = 1;   // set flag in ISR
}