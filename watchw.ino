/*
 * WatchW - Watch winder project 
 * Platform: Arduino UNO
 *  
 *  
 */

#include <MenuSystem.h>
#include <TimeLib.h>
#include "LCDRenderer.h"

//Debug flag on/off - comment/uncomment
#define DEBUG 1
#include "MDEbug.h"



//--------------------------------------------------------------------------------
//--------------------------CONST-------------------------------------------------
enum Buttons { 
   B_NONE = 0,
   B_UP,
   B_DOWN,
   B_LEFT,
   B_RIGHT,
   B_SELECT,
   B_NO_CHANGES
};

// Directions of wind
enum Direction {
  D_CW,     // Clockwise
  D_CCW,    // Counterclockwise 
  D_BOTH    // Both directions
};

// Various winding modes
enum WindMode {
  W_INIT,
  W_FULL,
  W_LONG,
  W_STOP
};
             

#define DEFAULT_TPD       200      // 850; //turns per day  to save the load in the same state
#define DEFAULT_FULL_T    100      // # of turns for full load     
#define MIN_DISCHARGE_PCT 99       //will stop winding till we will reach this percent of discharge 


//------------------------GLOBALS AND  INITIALIZATIONS---------------------------------------------------



//   The LCD circuit:
//    * LCD RS pin to digital pin 8
//    * LCD Enable pin to digital pin 9
//    * LCD D4 pin to digital pin 4
//    * LCD D5 pin to digital pin 5
//    * LCD D6 pin to digital pin 6
//    * LCD D7 pin to digital pin 7
//    * LCD R/W pin to ground
LiquidCrystal lcd = LiquidCrystal(8, 9, 4, 5, 6, 7);


LCDRenderer my_renderer(lcd);
MenuSystem ms(my_renderer); //menu


//TODO: make part of winder class 
int    g_direction             = D_CW;
int    g_tpd                   = DEFAULT_TPD;
int    g_turns_to_full         = DEFAULT_FULL_T; 
int    g_mode                  = W_INIT;
int    g_current_turns         = 0;
int    g_current_load_pct      = 0;
time_t g_last_turn_t           = 0; //time stamp of last turn
 


             
//--------------------------------------------------------------------------------
int readButtons() {
  static int last_read=B_NONE;
  int prev_read = last_read;
  int x;
  int res;

  delay(50); //debouncing of analog buttons ??? / TODO: check how to fix
  x = analogRead (0);
  
  if (x < 60) {
   last_read=B_RIGHT;
  }
  else if (x < 200) {
    last_read=B_UP;
  }
  else if (x < 400) {
   last_read=B_DOWN;
  }
  else if (x < 600) {
    last_read=B_LEFT;
  }
  else if (x < 800) {
    last_read=B_SELECT; 
  } else {
    last_read=B_NONE;
  }

  // Do not return the same value many times - seems that button was pressed too long 
  
  if (last_read) DPRINTLN(last_read);

  if(prev_read == last_read)
    return B_NO_CHANGES;
  else 
    return last_read;
  
}

//--------------------------------------------------------------------------------
void print_status(){
   char msg[17];
   static time_t last_status_update = 0;

   if ( (now() -  last_status_update)  > 1 ) {
      sprintf (msg,"WatchW %d|%d%%",g_current_turns, g_current_load_pct);
      ms.get_root_menu().set_name(msg);
      ms.display();
      last_status_update = now();
      //lcd.setCursor(,13);
      //lcd.print(g_current_load_pct); lcd.println("%");
   }

   
}

//--------------------------------------------------------------------------------
void do_one_turn(){
 
  
  //TODO: stepper motor perform one full turn
  DPRINT("Turn: ");
  DPRINTLN(g_current_turns);
  g_last_turn_t = now();
  
}

//--------------------------------------------------------------------------------
void do_wind () {

    //save time stamp when watch was fully loaded and winder was stoped  
    static time_t winder_sleep_t = 0;
   
     
    int tmp_turns;
    

    if(g_current_turns)
      g_current_load_pct =  (g_current_turns * 1.0 / g_turns_to_full) * 100;  
   
    
    switch (g_mode) {
      case W_INIT:          //
        break;
        
      case W_LONG:          //
        if(g_current_turns == 0)    // let's suppose 
           g_current_turns = g_turns_to_full/2;
        if( now() - g_last_turn_t > (24*3600.0)/g_tpd ){
          do_one_turn();
          //do not increase turn counter 
          //in this mode we just preserve the charge state
          //the charge of warch remains the same as it was  
         else {
          //skip turn
         }
        
        break;
      case W_FULL:          //     
        do_one_turn();
        g_current_turns++;  // charge watch by one turn;
        if(g_current_turns >= g_tpd) {
             DPRINTLN("Full load reached -> stop");
             g_mode = W_STOP;
             winder_sleep_t = now();
        }
        break;
        
      case W_STOP:        // 
        // calculate the discharge and decrease # of turns
        // 
        tmp_turns = g_turns_to_full - g_tpd * ((now() -  winder_sleep_t)   / (24*3600.0));
        g_current_turns = tmp_turns > 0 ? tmp_turns : g_current_turns; 

        // winder was stopped after being charged  
        // and discharged below min threshold – start again 
        if ( g_current_load_pct < MIN_DISCHARGE_PCT) {
            // discart stop timestemp
            winder_sleep_t = 0;
            //start LONG mode
            g_mode = W_LONG;
            DPRINTLN("Min load reached -> start again");
        }   
        break;
        
      default:
        break;
    }
    delay(100);
    print_status();
 
  }

//--------------------------------------------------------------------------------
// Menu callback function
void on_longw_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,1);
  lcd.print("Long winding ...  ");
  g_current_turns = 0;
  g_mode = W_LONG;
  delay(1500); // so we can look the result on the LCD
  
}

//--------------------------------------------------------------------------------
void on_fullw_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,1);
  lcd.print("Full winding ...  ");
  g_current_turns = 0;
  g_mode = W_FULL;
  delay(1500); // so we can look the result on the LCD
}

//--------------------------------------------------------------------------------
void on_dir_cw_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,1);
  lcd.print("Item 1_3 Selected  ");
  delay(1500); // so we can look the result on the LCD
}

//--------------------------------------------------------------------------------
void on_dir_ccw_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,1);
  lcd.print("Item 1_3 Selected  ");
  delay(1500); // so we can look the result on the LCD
}

//--------------------------------------------------------------------------------
void on_dir_both_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,1);
  lcd.print("Item 1_3 Selected  ");
  delay(1500); // so we can look the result on the LCD
}

//--------------------------------------------------------------------------------
void on_tpd_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,1);
  lcd.print("Item 2_1 Selected  ");
  delay(1500); // so we can look the result on the LCD
}

//--------------------------------------------------------------------------------
void on_stat_selected(MenuItem* p_menu_item)
{
  lcd.setCursor(0,1);
  lcd.print("Item 2_2  Selected  ");
  delay(1500); // so we can look the result on the LCD
}

//--------------------------------------------------------------------------------
void serialPrintHelp() {
  Serial.println("***************");
  Serial.println("w: go to previus item (up)");
  Serial.println("s: go to next item (down)");
  Serial.println("a: go back (right)");
  Serial.println("d: select \"selected\" item");
  Serial.println("?: print this help");
  Serial.println("h: print this help");
  Serial.println("***************");
}

//--------------------------------------------------------------------------------
void serialHandler() {
  char inChar;
  if((inChar = Serial.read())>0) {
    switch (inChar) {
    case 'w': // Previus item
      ms.prev();
      ms.display();
      break;
    case 's': // Next item
      ms.next();
      ms.display();
      break;
    case 'a': // Back presed
      ms.back();
      ms.display();
      break;
    case 'd': // Select presed
      ms.select();
      ms.display();
      break;
    case '?':
    case 'h': // Display help
      serialPrintHelp();
      break;
    default:
      break;
    }
  }
}

//--------------------------------------------------------------------------------
void buttonHandler() {
  int but = readButtons();
  if(but>0) {
    switch (but) {
    case B_NO_CHANGES:
      //do nothing
      break;  
    case B_LEFT: // Previus item
      ms.prev();
      ms.display();
      break;
    case B_RIGHT: // Next item
      ms.next();
      ms.display();
      break;
    case B_UP: // Back presed
      ms.back();
      ms.display();
      break;
    case B_DOWN:
      //ms.back();
      //ms.display();
      break;
    case B_SELECT: // Select presed
      ms.select();
      ms.display();
      break;
    default:
      break;
    }
  }
}

// Standard arduino functions
void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);

  serialPrintHelp();

  /* MENU 
    -Long wind  // watches are always ready and almost full  
    -Full wind  // make it fast ready from 0 - make full wind cycle asap
    -Configure  // configuration menu
    --Direction (CW,CCW,Both) // choose direction clockwise,counterclockwise,both
    --Turn per day (TPD - number of turns per day)
    -Statistics - Turn counter/ % of ready  - displays number of turns since start.
  */
 
  
  // Menu variables
  MenuItem mm_mi1("Long wind",     &on_longw_selected);
  MenuItem mm_mi2("Full wind",     &on_fullw_selected);
  MenuItem mm_mi3("Statistics",    &on_stat_selected);
  Menu mu1("Configuration");
  Menu mu2("Direction");
  MenuItem mu2_mi1("CW",    &on_dir_cw_selected);
  MenuItem mu2_mi2("CCW",   &on_dir_ccw_selected);
  MenuItem mu2_mi3("BOTH",  &on_dir_both_selected);
  MenuItem mu1_mi1("Turn per day", &on_tpd_selected);

  //setup menu
  ms.get_root_menu().set_name("WatchW");
  ms.get_root_menu().add_item(&mm_mi1);
  ms.get_root_menu().add_item(&mm_mi2);
  ms.get_root_menu().add_item(&mm_mi3);
  ms.get_root_menu().add_menu(&mu1);
  
  mu1.add_menu(&mu2);
  mu1.add_item(&mu1_mi1);
  
  mu2.add_item(&mu2_mi1);
  mu2.add_item(&mu2_mi2);
  mu2.add_item(&mu2_mi3);

  ms.display();
}

//------------------------------------------------------------------------------
void loop()
{
  serialHandler();
  buttonHandler();
  do_wind();
}
