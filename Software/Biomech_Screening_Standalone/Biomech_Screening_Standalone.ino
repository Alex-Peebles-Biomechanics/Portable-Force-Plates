#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

// Temporary calibration coefficients, will set later (unit N/ADC)
float m_L_TL = 0.9042, m_L_BL = 0.9184, m_L_BR = 0.9005, m_L_TR = 0.9200;
float m_R_TL = 0.9169, m_R_BL = 0.8968, m_R_BR = 0.8882, m_R_TR = 0.9112;

const int L_TL_Pin = A0;
const int L_BL_Pin = A1;
const int L_BR_Pin = A2;
const int L_TR_Pin = A3;
const int R_TL_Pin = A4;
const int R_BL_Pin = A5;
const int R_BR_Pin = A6;
const int R_TR_Pin = A7;

const int enterButton = 4;
const int CLK = 3;
const int DT =  2;
int currentStateA;
int lastStateA;

const int GreenLED = 7;
const int buzzerPin = 6;
const int RedLED = 5;

const int debounce = 250;
const int IC_Thresh = 125;
const int TO_Thresh = 100;

float L_TL_0, L_BL_0, L_BR_0, L_TR_0, R_TL_0, R_BL_0, R_BR_0, R_TR_0;
float L_TL, L_BL, L_BR, L_TR, R_TL, R_BL, R_BR, R_TR;

int DominantLimb = 1; // 1-right, 2-left
int testPlate = 1;  // 1-right, 2-left

int Fs = 500;
float bodyweight = 1;
int trialLength = 20;
String cmd;
unsigned long previousMillis, startingMillis;

float PlateNumberChoices[] = {1, 1, 22};
int leftPlateSelect = 1;
int rightPlateSelect = 1;

const char *mainMenuChoices[] = {"1. New Subject", "2. Tare plates", "3. Display force", "4. Display raw"};
int mainMenuSelect = 1;

// const char *movementMenuChoices[] = {"1. Balance - EO", "2. Balance - EC", "3. Squat", "4. DJ vert jump", "5. DL DVJ", "6. DL drop landing", "7. SL vert jump", "8. SL DVJ", "9. SL drop landing","Exit"};
const char *movementMenuChoices[] = {"1. Balance - EO", "2. Balance - EC", "3. Squat", "Exit"};
String movementChoice;

int  MovementSelected = 1, mainMenu = 1, trainingMenuSelect = 1, screenStartChoice = 1, trainStartChoice = 1;
bool updateLCD = true, passed_Trial = true;


float leftForce, rightForce, totalForce, Time, COPx, COPy, COPx_prev, COPy_prev, pathLength, swayVel;
float length = 15.875, width = 12.75;
float TL, BL, BR, TR, AveSym, sym, PkForce;
int state, count, page, N;
bool IC_Prev1;

void setup() {
  delay(1000);
  analogReadResolution(12);
  
  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(enterButton, INPUT);
  lastStateA = digitalRead(CLK);
  
  pinMode(GreenLED,OUTPUT);
  pinMode(RedLED, OUTPUT);
    
  lcd.begin(20, 4);
  lcd.blink();
  
  leftPlateSelect = FloatMenu("Left Plate Number:", " ", PlateNumberChoices, leftPlateSelect);
  rightPlateSelect = FloatMenu("Right Plate Number:", " ", PlateNumberChoices, rightPlateSelect);
  updateSlopes();
  CalibrateForcePlates();

}


void loop() {
  mainMenuSelect = StringMenu("Select:", mainMenuChoices, 4, mainMenuSelect); 
    if (mainMenuSelect == 1){
      Screen_1();
    } else if (mainMenuSelect == 2) {
      CalibrateForcePlates();
    } else if (mainMenuSelect == 3) {
      displayForce();
    } else if (mainMenuSelect == 4) {
      displayRaw();
    }  
}
