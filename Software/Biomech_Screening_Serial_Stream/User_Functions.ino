void Screen_1() { // Record limb and weight.
  // studySelect = TwoStringMenu("Select:", studySelectChoices); 
  PIDSelect = FloatMenu("Enter PID:", " ", PIDChoices, PIDSelect);
  // DominantLimb = TwoStringMenu("Set Dominant Limb:", limbMenuChoices);
  // recordBodyweight();
  Screen_2();
}

void Screen_2() { // select movement
  MovementSelected = StringMenu("Select movement:", movementMenuChoices, 4, MovementSelected);
  movementChoice = (String) movementMenuChoices[MovementSelected - 1];
  movementChoice.remove(0, 3);

    if (MovementSelected == 1 || MovementSelected == 2) { // balance task
      Fs = 100;
      Screen_Balance();
    } else if (MovementSelected == 4){
      // should exit back to start menu (new subject, etc)
    } else { // squatting task..
      Fs = 500;
      Screen_Force();
    }
}

void Screen_Balance(){
  lcd.clear();
  lcd.setCursor(0, 0);
  if (MovementSelected == 1){
    lcd.print("Balance: Eyes Open");
  } else {
    lcd.print("Balance: Eyes Closed");
  }
  lcd.setCursor(0, 1);
  lcd.print("Stand on one plate");
  lcd.setCursor(0, 2);
  lcd.print("and press enter to");
  lcd.setCursor(0, 3);
  lcd.print("begin 30 sec trial");
 
  while (digitalRead(enterButton) == LOW) {}
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce);

  lcd.clear();
  lcd.setCursor(0, 0);
  readForce();
  if ((R_TL + R_BL + R_BR + R_TR) > (L_TL + L_BL + L_BR + L_TR)){
    testPlate = 1;
    lcd.print("Testing Right Plate");
  } else {
    testPlate = 2;
    lcd.print("Testing Left Plate");
  }

  struct datastoreCOP startMessage;
  startMessage.Time = studySelect;
  startMessage.COPx = PIDSelect;
  startMessage.COPy = MovementSelected;
  Serial.write((const uint8_t *)&startMessage, sizeof(startMessage));

  trialLength = 30;
  int count;
  float COPx, COPy, TL, TR, BL, BR;
  float length = 15.875, width = 12.75;
  startingMillis = millis();
  previousMillis = millis();
  while ((int) (millis() - startingMillis) < (trialLength * 1000)) {
    
    count = 0, TL=0, TR=0, BL=0, BR=0;
    while ((millis() - previousMillis) < (1000.0/Fs)) {
     readForce();
     if (testPlate == 1) {
        TL += R_TL;
        BL += R_BL;
        BR += R_BR;
        TR += R_TR;
        count++;
      } else {
        TL += L_TL;
        BL += L_BL;
        BR += L_BR;
        TR += L_TR;
        count++;
      }           
    }    
    previousMillis = millis();

    TL = TL/count;
    BL = BL/count;
    BR = BR/count;
    TR = TR/count;

    COPx = ((width * TL + width * BL - width * BR - width * TR) / (TL + BL + BR + TR));
    COPy = ((length * TL - length * BL - length * BR + length * TR) / (TL + BL + BR + TR));
    
    struct datastoreCOP myData;
    myData.Time = (float) (millis() - startingMillis);
    myData.COPx = COPx;
    myData.COPy = COPy;
    Serial.write((const uint8_t *)&myData, sizeof(myData));
  }

  struct datastoreCOP exitMessage;
  exitMessage.Time = 0;
  exitMessage.COPx = 0;
  exitMessage.COPy = 0;
  Serial.write((const uint8_t *)&exitMessage, sizeof(exitMessage));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for results");

  while (Serial.available() == 0) {}
  lcd.clear();
  lcd.setCursor(0, 0);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  lcd.setCursor(0, 1);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  lcd.setCursor(0, 2);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  lcd.setCursor(0, 3);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  
  while (digitalRead(enterButton) == LOW) {}
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce);
  
  Screen_2();
}

void Screen_Force(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bodyweight Squats");
  lcd.setCursor(0, 1);
  lcd.print("Five reps, as low as");
  lcd.setCursor(0, 2);
  lcd.print("you can safely.");
  lcd.setCursor(0, 3);
  lcd.print("Press enter to begin");
  
  while (digitalRead(enterButton) == LOW) {}
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("TESTING...");
  lcd.setCursor(0, 1);
  lcd.print("Press enter to stop");
  lcd.setCursor(0, 2);
  
  struct datastoreForce startMessage;
  startMessage.Time = studySelect;
  startMessage.leftForce = PIDSelect;
  startMessage.rightForce = MovementSelected;
  Serial.write((const uint8_t *)&startMessage, sizeof(startMessage));

  int count;
  float leftForce, rightForce;
  startingMillis = millis();
  while (digitalRead(enterButton) == LOW) {
    
    count = 0, rightForce=0, leftForce=0;
    previousMillis = millis();
    while ((millis() - previousMillis) < (1 / ((double) Fs / 1000))) {
     readForce();
     leftForce += (L_TL + L_BL + L_BR + L_TR);
     rightForce += (R_TL + R_BL + R_BR + R_TR);
     count++; 
    }
    
    struct datastoreForce myData;
    myData.Time = (float) (millis() - startingMillis);
    myData.leftForce = leftForce/count;
    myData.rightForce = rightForce/count;
    Serial.write((const uint8_t *)&myData, sizeof(myData));
  }
  while (digitalRead(enterButton) == HIGH) {}
  delay(3*debounce);

  struct datastoreForce exitMessage;
  exitMessage.Time = 0;
  exitMessage.leftForce = 0;
  exitMessage.rightForce = 0;
  Serial.write((const uint8_t *)&exitMessage, sizeof(exitMessage));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waiting for results"); 

  while (Serial.available() == 0) {}
  lcd.clear();
  lcd.setCursor(0, 0);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  lcd.setCursor(0, 1);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  lcd.setCursor(0, 2);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  lcd.setCursor(0, 3);
  incomingMsg = Serial.readStringUntil(',');
  lcd.print(incomingMsg);
  
  while (digitalRead(enterButton) == LOW) {}
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce);
  
  Screen_2();
}

void recordBodyweight() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Stand on one plate");
  lcd.setCursor(0, 1);
  lcd.print("and press enter");
  lcd.setCursor(0, 2);
  lcd.print("to record bodyweight");
  while (digitalRead(enterButton) == LOW) {}
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce);
  
  L_TL = 0, L_BL = 0, L_BR = 0, L_TR = 0, R_TL = 0, R_BL = 0, R_BR = 0, R_TR = 0;
  count = 0;
  while(count < 100) {
    L_TL += ((float) analogRead(L_TL_Pin)) * m_L_TL + L_TL_0;
    L_BL += ((float) analogRead(L_BL_Pin)) * m_L_BL + L_BL_0;
    L_BR += ((float) analogRead(L_BR_Pin)) * m_L_BR + L_BR_0;
    L_TR += ((float) analogRead(L_TR_Pin)) * m_L_TR + L_TR_0;
    R_TL += ((float) analogRead(R_TL_Pin)) * m_R_TL + R_TL_0;
    R_BL += ((float) analogRead(R_BL_Pin)) * m_R_BL + R_BL_0;
    R_BR += ((float) analogRead(R_BR_Pin)) * m_R_BR + R_BR_0;
    R_TR += ((float) analogRead(R_TR_Pin)) * m_R_TR + R_TR_0;
    count++;
  }
  
  struct datastoreForce BW_Message;
  BW_Message.Time = 0;
  BW_Message.leftForce = (L_TL + L_BL + L_BR + L_TR) / count;
  BW_Message.rightForce = (R_TL + R_BL + R_BR + R_TR) / count;
  Serial.write((const uint8_t *)&BW_Message, sizeof(BW_Message));

  if (BW_Message.leftForce > BW_Message.rightForce){
    bodyweight = BW_Message.leftForce;
  } else {
    bodyweight = BW_Message.rightForce;
  }

  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("BW: " + (String) (int) (bodyweight*0.225) + " lb");
  lcd.setCursor(0, 1);
  lcd.print("Enter to accept");

  while (digitalRead(enterButton) == LOW) {}
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce);
}

void updateSlopes(){
  if (leftPlateSelect == 1){
    m_L_TL = 	0.9526	, m_L_BL =	0.9470	,	m_L_BR =	0.9686	,	m_L_TR = 	0.9410	;
  } else if (leftPlateSelect == 2){
    m_L_TL =	0.9270	,	m_L_BL =	0.9408	,	m_L_BR =	0.9643	,	m_L_TR =	0.9239	;
  } else if (leftPlateSelect == 3){
    m_L_TL =	0.9461	,	m_L_BL =	0.9408	,	m_L_BR =	0.9236	,	m_L_TR =	0.9518	;
  } else if (leftPlateSelect == 4){
    m_L_TL =	0.9470	,	m_L_BL =	0.9402	,	m_L_BR =	0.9366	,	m_L_TR =	0.9459	;
  } else if (leftPlateSelect == 5){
    m_L_TL =	0.9234	,	m_L_BL =	0.9172	,	m_L_BR =	0.9141	,	m_L_TR =	0.8958	;
  } else if (leftPlateSelect == 6){
    m_L_TL =	0.8994	,	m_L_BL =	0.9003	,	m_L_BR =	0.9295	,	m_L_TR =	0.9013	;
  } else if (leftPlateSelect == 7){
    m_L_TL =	0.9019	,	m_L_BL =	0.8951	,	m_L_BR =	0.9199	,	m_L_TR =	0.8888	;
  } else if (leftPlateSelect == 8){
    m_L_TL =	0.8865	,	m_L_BL =	0.9060	,	m_L_BR =	0.9143	,	m_L_TR =	0.9461	;
  } else if (leftPlateSelect == 9){
    m_L_TL =	0.9385	,	m_L_BL =	0.9282	,	m_L_BR =	0.9125	,	m_L_TR =	0.9033	;
  } else if (leftPlateSelect == 10){
    m_L_TL =	0.9217	,	m_L_BL =	0.8965	,	m_L_BR =	0.8974	,	m_L_TR =	0.9081	;
  } else if (leftPlateSelect == 11){
    m_L_TL =	0.8993	,	m_L_BL =	0.9333	,	m_L_BR =	0.9140	,	m_L_TR =	0.9255	;
  } else if (leftPlateSelect == 12){
    m_L_TL =	0.9381	,	m_L_BL =	0.9250	,	m_L_BR =	0.9270	,	m_L_TR =	0.9066	;
  } else if (leftPlateSelect == 13){
    m_L_TL =	0.9139	,	m_L_BL =	0.8901	,	m_L_BR =	0.9006	,	m_L_TR =	0.9029	;
  } else if (leftPlateSelect == 14){
    m_L_TL =	0.8925	,	m_L_BL =	0.9056	,	m_L_BR =	0.9123	,	m_L_TR =	0.9125	;
  } else if (leftPlateSelect == 15){
    m_L_TL =	0.8984	,	m_L_BL =	0.8909	,	m_L_BR =	0.9207	,	m_L_TR =	0.9132	;
  } else if (leftPlateSelect == 16){
    m_L_TL =	0.9047	,	m_L_BL =	0.9144	,	m_L_BR =	0.9093	,	m_L_TR =	0.9048	;
  } else if (leftPlateSelect == 17){
    m_L_TL =	0.9042	,	m_L_BL =	0.9184	,	m_L_BR =	0.9005	,	m_L_TR =	0.9200	;
  } else if (leftPlateSelect == 18){
    m_L_TL =	0.9169	,	m_L_BL =	0.8968	,	m_L_BR =	0.8882	,	m_L_TR =	0.9112	;
  } else if (leftPlateSelect == 19){
    m_L_TL =	0.9037	,	m_L_BL =	0.9005	,	m_L_BR =	0.9127	,	m_L_TR =	0.9166	;
  } else if (leftPlateSelect == 20){
    m_L_TL =	0.8815	,	m_L_BL =	0.9290	,	m_L_BR =	0.8981	,	m_L_TR =	0.9040	;
  } else if (leftPlateSelect == 21){
    m_L_TL =	0.9670	,	m_L_BL =	0.9624  ,	m_L_BR =	0.9578	,	m_L_TR =	0.9464	;
  } else if (leftPlateSelect == 22){
    m_L_TL =	0.9670	,	m_L_BL =	0.9765	,	m_L_BR =	0.9647	,	m_L_TR =	0.9765	;
  }

  if (rightPlateSelect == 1){
    m_R_TL = 	0.9526	, m_R_BL =	0.9470	,	m_R_BR =	0.9686	,	m_R_TR = 	0.9410	;
  } else if (rightPlateSelect == 2){
    m_R_TL =	0.9270	,	m_R_BL =	0.9408	,	m_R_BR =	0.9643	,	m_R_TR =	0.9239	;
  } else if (rightPlateSelect == 3){
    m_R_TL =	0.9461	,	m_R_BL =	0.9408	,	m_R_BR =	0.9236	,	m_R_TR =	0.9518	;
  } else if (rightPlateSelect == 4){
    m_R_TL =	0.9470	,	m_R_BL =	0.9402	,	m_R_BR =	0.9366	,	m_R_TR =	0.9459	;
  } else if (rightPlateSelect == 5){
    m_R_TL =	0.9234	,	m_R_BL =	0.9172	,	m_R_BR =	0.9141	,	m_R_TR =	0.8958	;
  } else if (rightPlateSelect == 6){
    m_R_TL =	0.8994	,	m_R_BL =	0.9003	,	m_R_BR =	0.9295	,	m_R_TR =	0.9013	;
  } else if (rightPlateSelect == 7){
    m_R_TL =	0.9019	,	m_R_BL =	0.8951	,	m_R_BR =	0.9199	,	m_R_TR =	0.8888	;
  } else if (rightPlateSelect == 8){
    m_R_TL =	0.8865	,	m_R_BL =	0.9060	,	m_R_BR =	0.9143	,	m_R_TR =	0.9461	;
  } else if (rightPlateSelect == 9){
    m_R_TL =	0.9385	,	m_R_BL =	0.9282	,	m_R_BR =	0.9125	,	m_R_TR =	0.9033	;
  } else if (rightPlateSelect == 10){
    m_R_TL =	0.9217	,	m_R_BL =	0.8965	,	m_R_BR =	0.8974	,	m_R_TR =	0.9081	;
  } else if (rightPlateSelect == 11){
    m_R_TL =	0.8993	,	m_R_BL =	0.9333	,	m_R_BR =	0.9140	,	m_R_TR =	0.9255	;
  } else if (rightPlateSelect == 12){
    m_R_TL =	0.9381	,	m_R_BL =	0.9250	,	m_R_BR =	0.9270	,	m_R_TR =	0.9066	;
  } else if (rightPlateSelect == 13){
    m_R_TL =	0.9139	,	m_R_BL =	0.8901	,	m_R_BR =	0.9006	,	m_R_TR =	0.9029	;
  } else if (rightPlateSelect == 14){
    m_R_TL =	0.8925	,	m_R_BL =	0.9056	,	m_R_BR =	0.9123	,	m_R_TR =	0.9125	;
  } else if (rightPlateSelect == 15){
    m_R_TL =	0.8984	,	m_R_BL =	0.8909	,	m_R_BR =	0.9207	,	m_R_TR =	0.9132	;
  } else if (rightPlateSelect == 16){
    m_R_TL =	0.9047	,	m_R_BL =	0.9144	,	m_R_BR =	0.9093	,	m_R_TR =	0.9048	;
  } else if (rightPlateSelect == 17){
    m_R_TL =	0.9042	,	m_R_BL =	0.9184	,	m_R_BR =	0.9005	,	m_R_TR =	0.9200	;
  } else if (rightPlateSelect == 18){
    m_R_TL =	0.9169	,	m_R_BL =	0.8968	,	m_R_BR =	0.8882	,	m_R_TR =	0.9112	;
  } else if (rightPlateSelect == 19){
    m_R_TL =	0.9037	,	m_R_BL =	0.9005	,	m_R_BR =	0.9127	,	m_R_TR =	0.9166	;
  } else if (rightPlateSelect == 20){
    m_R_TL =	0.8815	,	m_R_BL =	0.9290	,	m_R_BR =	0.8981	,	m_R_TR =	0.9040	;
  } else if (rightPlateSelect == 21){
    m_R_TL =	0.9670	,	m_R_BL =	0.9624  ,	m_R_BR =	0.9578	,	m_R_TR =	0.9464	;
  } else if (rightPlateSelect == 22){
    m_R_TL =	0.9670	,	m_R_BL =	0.9765	,	m_R_BR =	0.9647	,	m_R_TR =	0.9765	;
  }

}

void readForce() {
  L_TL = ((float) analogRead(L_TL_Pin)) * m_L_TL + L_TL_0;
  L_BL = ((float) analogRead(L_BL_Pin)) * m_L_BL + L_BL_0;
  L_BR = ((float) analogRead(L_BR_Pin)) * m_L_BR + L_BR_0;
  L_TR = ((float) analogRead(L_TR_Pin)) * m_L_TR + L_TR_0;
  R_TL = ((float) analogRead(R_TL_Pin)) * m_R_TL + R_TL_0;
  R_BL = ((float) analogRead(R_BL_Pin)) * m_R_BL + R_BL_0;
  R_BR = ((float) analogRead(R_BR_Pin)) * m_R_BR + R_BR_0;
  R_TR = ((float) analogRead(R_TR_Pin)) * m_R_TR + R_TR_0;
}

void CalibrateForcePlates() {
  //F = m*A + A0
  //A0 = -m*A(F0)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Clear plates and");
  lcd.setCursor(0, 1);
  lcd.print("press enter");
  while (digitalRead(enterButton) == LOW) {}
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce);
  
  L_TL = 0, L_BL = 0, L_BR = 0, L_TR = 0, R_TL = 0, R_BL = 0, R_BR = 0, R_TR = 0;
  count = 0;
  while (count < 250) {
    L_TL += analogRead(L_TL_Pin);
    L_BL += analogRead(L_BL_Pin);
    L_BR += analogRead(L_BR_Pin);
    L_TR += analogRead(L_TR_Pin);
    R_TL += analogRead(R_TL_Pin);
    R_BL += analogRead(R_BL_Pin);
    R_BR += analogRead(R_BR_Pin);
    R_TR += analogRead(R_TR_Pin);
    count++;
    delay(5);
  }

  L_TL_0 = -m_L_TL * ((float) L_TL) / count; // divide by ten b/c you are summing ten readings...
  L_BL_0 = -m_L_BL * ((float) L_BL) / count;
  L_BR_0 = -m_L_BR * ((float) L_BR) / count;
  L_TR_0 = -m_L_TR * ((float) L_TR) / count;
  R_TL_0 = -m_R_TL * ((float) R_TL) / count;
  R_BL_0 = -m_R_BL * ((float) R_BL) / count;
  R_BR_0 = -m_R_BR * ((float) R_BR) / count;
  R_TR_0 = -m_R_TR * ((float) R_TR) / count;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Plates have been");
  lcd.setCursor(0, 1);
  lcd.print("Sucessfully zeroed");
  delay(1000);
    
}
void displayForce() {
  updateLCD = true;
  while (digitalRead(enterButton) == LOW) {
    currentStateA = digitalRead(CLK);
    if (currentStateA != lastStateA  && currentStateA == 1){
      updateLCD = true;
    }
    lastStateA = currentStateA;
    delay(1);

    if (updateLCD) {
    int L_TL_A = 0, L_BL_A = 0, L_BR_A = 0, L_TR_A = 0, R_TL_A = 0, R_BL_A = 0, R_BR_A = 0, R_TR_A = 0;
    count = 0;
    for (int i = 1; i <= 250; i++) {
      L_TL_A += analogRead(L_TL_Pin);
      L_BL_A += analogRead(L_BL_Pin);
      L_BR_A += analogRead(L_BR_Pin);
      L_TR_A += analogRead(L_TR_Pin);
      R_TL_A += analogRead(R_TL_Pin);
      R_BL_A += analogRead(R_BL_Pin);
      R_BR_A += analogRead(R_BR_Pin);
      R_TR_A += analogRead(R_TR_Pin);
      count++;
      delay(5);
    }

    double L_TL_A_Ave, L_BL_A_Ave, L_BR_A_Ave, L_TR_A_Ave, L_TL_F_Ave, L_BL_F_Ave, L_BR_F_Ave, L_TR_F_Ave, L_Force, L_Analog;

    L_TL_A_Ave = (double) (L_TL_A) / count;
    L_BL_A_Ave = (double) (L_BL_A) / count;
    L_BR_A_Ave = (double) (L_BR_A) / count;
    L_TR_A_Ave = (double) (L_TR_A) / count;

    L_Analog = (L_TL_A_Ave + L_BL_A_Ave + L_BR_A_Ave + L_TR_A_Ave);

    L_TL_F_Ave = L_TL_A_Ave * m_L_TL + L_TL_0;
    L_BL_F_Ave = L_BL_A_Ave * m_L_BL + L_BL_0;
    L_BR_F_Ave = L_BR_A_Ave * m_L_BR + L_BR_0;
    L_TR_F_Ave = L_TR_A_Ave * m_L_TR + L_TR_0;

    L_Force = (L_TL_F_Ave + L_BL_F_Ave + L_BR_F_Ave + L_TR_F_Ave);

    double R_TL_A_Ave, R_BL_A_Ave, R_BR_A_Ave, R_TR_A_Ave, R_TL_F_Ave, R_BL_F_Ave, R_BR_F_Ave, R_TR_F_Ave, R_Force, R_Analog;

    R_TL_A_Ave = (double) (R_TL_A) / count;
    R_BL_A_Ave = (double) (R_BL_A) / count;
    R_BR_A_Ave = (double) (R_BR_A) / count;
    R_TR_A_Ave = (double) (R_TR_A) / count;

    R_Analog = (R_TL_A_Ave + R_BL_A_Ave + R_BR_A_Ave + R_TR_A_Ave);

    R_TL_F_Ave = R_TL_A_Ave * m_R_TL + R_TL_0;
    R_BL_F_Ave = R_BL_A_Ave * m_R_BL + R_BL_0;
    R_BR_F_Ave = R_BR_A_Ave * m_R_BR + R_BR_0;
    R_TR_F_Ave = R_TR_A_Ave * m_R_TR + R_TR_0;

    R_Force = (R_TL_F_Ave + R_BL_F_Ave + R_BR_F_Ave + R_TR_F_Ave);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LF: " + (String) (int) (L_Force) + " RF: " + (String) (int) (R_Force));
    lcd.setCursor(0, 1);
    lcd.print("LA: " + (String) (int) (L_Analog) + " RA: " + (String) (int) (R_Analog));
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("L_TL:" + (String) (int) (L_TL_A_Ave) + " L_BL:" + (String) (int) (L_BL_A_Ave));
    // lcd.setCursor(0, 1);
    // lcd.print("L_BR:" + (String) (int) (L_BR_A_Ave) + " L_TR:" + (String) (int) (L_TR_A_Ave));
    // lcd.setCursor(0, 2);
    // lcd.print("R_TL:" + (String) (int) (R_TL_A_Ave) + " R_BL:" + (String) (int) (R_BL_A_Ave));
    // lcd.setCursor(0, 3);
    // lcd.print("R_BR:" + (String) (int) (L_BR_A_Ave) + " R_TR:" + (String) (int) (R_TR_A_Ave));
    updateLCD = false;
    }
  }
  while(digitalRead(enterButton) == HIGH){}
  delay(debounce);
}

void displayRaw() {
  updateLCD = true;
  while (digitalRead(enterButton) == LOW) {
    currentStateA = digitalRead(CLK);
    if (currentStateA != lastStateA  && currentStateA == 1){
      updateLCD = true;
    }
    lastStateA = currentStateA;
    delay(1);

    if (updateLCD) {
    int L_TL_A = 0, L_BL_A = 0, L_BR_A = 0, L_TR_A = 0, R_TL_A = 0, R_BL_A = 0, R_BR_A = 0, R_TR_A = 0;
    count = 0;
    for (int i = 1; i <= 250; i++) {
      L_TL_A += analogRead(L_TL_Pin);
      L_BL_A += analogRead(L_BL_Pin);
      L_BR_A += analogRead(L_BR_Pin);
      L_TR_A += analogRead(L_TR_Pin);
      R_TL_A += analogRead(R_TL_Pin);
      R_BL_A += analogRead(R_BL_Pin);
      R_BR_A += analogRead(R_BR_Pin);
      R_TR_A += analogRead(R_TR_Pin);
      count++;
      delay(5);
    }

    double L_TL_A_Ave, L_BL_A_Ave, L_BR_A_Ave, L_TR_A_Ave, L_TL_F_Ave, L_BL_F_Ave, L_BR_F_Ave, L_TR_F_Ave, L_Force, L_Analog;

    L_TL_A_Ave = (double) (L_TL_A) / count;
    L_BL_A_Ave = (double) (L_BL_A) / count;
    L_BR_A_Ave = (double) (L_BR_A) / count;
    L_TR_A_Ave = (double) (L_TR_A) / count;

    L_Analog = (L_TL_A_Ave + L_BL_A_Ave + L_BR_A_Ave + L_TR_A_Ave);

    L_TL_F_Ave = L_TL_A_Ave * m_L_TL + L_TL_0;
    L_BL_F_Ave = L_BL_A_Ave * m_L_BL + L_BL_0;
    L_BR_F_Ave = L_BR_A_Ave * m_L_BR + L_BR_0;
    L_TR_F_Ave = L_TR_A_Ave * m_L_TR + L_TR_0;

    L_Force = (L_TL_F_Ave + L_BL_F_Ave + L_BR_F_Ave + L_TR_F_Ave);

    double R_TL_A_Ave, R_BL_A_Ave, R_BR_A_Ave, R_TR_A_Ave, R_TL_F_Ave, R_BL_F_Ave, R_BR_F_Ave, R_TR_F_Ave, R_Force, R_Analog;

    R_TL_A_Ave = (double) (R_TL_A) / count;
    R_BL_A_Ave = (double) (R_BL_A) / count;
    R_BR_A_Ave = (double) (R_BR_A) / count;
    R_TR_A_Ave = (double) (R_TR_A) / count;

    R_Analog = (R_TL_A_Ave + R_BL_A_Ave + R_BR_A_Ave + R_TR_A_Ave);

    R_TL_F_Ave = R_TL_A_Ave * m_R_TL + R_TL_0;
    R_BL_F_Ave = R_BL_A_Ave * m_R_BL + R_BL_0;
    R_BR_F_Ave = R_BR_A_Ave * m_R_BR + R_BR_0;
    R_TR_F_Ave = R_TR_A_Ave * m_R_TR + R_TR_0;

    R_Force = (R_TL_F_Ave + R_BL_F_Ave + R_BR_F_Ave + R_TR_F_Ave);
    
    // lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.print("LF: " + (String) (int) (L_Force) + " RF: " + (String) (int) (R_Force));
    // lcd.setCursor(0, 1);
    // lcd.print("LA: " + (String) (int) (L_Analog) + " RA: " + (String) (int) (R_Analog));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("L_TL:" + (String) (int) (L_TL_A_Ave) + " L_BL:" + (String) (int) (L_BL_A_Ave));
    lcd.setCursor(0, 1);
    lcd.print("L_BR:" + (String) (int) (L_BR_A_Ave) + " L_TR:" + (String) (int) (L_TR_A_Ave));
    lcd.setCursor(0, 2);
    lcd.print("R_TL:" + (String) (int) (R_TL_A_Ave) + " R_BL:" + (String) (int) (R_BL_A_Ave));
    lcd.setCursor(0, 3);
    lcd.print("R_BR:" + (String) (int) (R_BR_A_Ave) + " R_TR:" + (String) (int) (R_TR_A_Ave));
    updateLCD = false;
    }
  }
  while(digitalRead(enterButton) == HIGH){}
  delay(debounce);
}

int StringMenu(String message, const char *choices[], int NumberOfChoices, int currentChoice) {
  int lineNumber = 1, pageNumber = 1;
  if (currentChoice <= 3) {
    if (currentChoice == 1){
      lineNumber = 1, pageNumber = 1;
    } else if (currentChoice == 2) {
      lineNumber = 2, pageNumber = 1;
    } else if (currentChoice == 3) {
      lineNumber = 3, pageNumber = 1;
    }
  } else if (currentChoice == 4) {
    lineNumber = 3, pageNumber = 2;
  } else if (currentChoice == 5) {
    lineNumber = 3, pageNumber = 3;
  } else if (currentChoice == 6) {
    lineNumber = 3, pageNumber = 4;
  } else if (currentChoice == 7) {
    lineNumber = 3, pageNumber = 5;
  } else {
    lineNumber = 3, pageNumber = 6;
  }
    
  updateLCD = true;
  while (digitalRead(enterButton) == LOW) {
   currentStateA = digitalRead(CLK);
    if (currentStateA != lastStateA  && currentStateA == 1){
      if (digitalRead(DT) == currentStateA) { // Clockwise
        if (lineNumber < 3) {
          lineNumber++;
        } else {
          pageNumber++;
          if ((pageNumber+2) > NumberOfChoices){
            pageNumber = 1, lineNumber = 1;
          }
        }
      } else { // Counterclockwise
        if (lineNumber > 1) {
          lineNumber--;
        } else {
          pageNumber--;
          if (pageNumber < 1){
            pageNumber = NumberOfChoices-2;
            lineNumber = 3;
          }
        }
      }
      updateLCD = true;
    }
    lastStateA = currentStateA;
    delay(1);

    if (updateLCD == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(message);
      lcd.setCursor(0, 1);
      lcd.print(choices[pageNumber-1]);
      lcd.setCursor(0, 2);
      lcd.print(choices[pageNumber]);
      lcd.setCursor(0, 3);
      lcd.print(choices[pageNumber+1]);
      lcd.setCursor(0, lineNumber);
      updateLCD = false;      
    }
  }
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce); 
    
  return pageNumber + lineNumber - 1;
}

int TwoStringMenu(String message, const char *choices[]) {
  int Selection = 1;
  updateLCD = true;
  while (digitalRead(enterButton) == LOW) {
   currentStateA = digitalRead(CLK);
    if (currentStateA != lastStateA  && currentStateA == 1){
      if (Selection == 1) { 
        Selection = 2;
      } else {
        Selection = 1;
      }
      updateLCD = true;
    }
    lastStateA = currentStateA;
    delay(1);

    if (updateLCD == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(message);
      lcd.setCursor(0, 1);
      lcd.print(choices[0]);
      lcd.setCursor(0, 2);
      lcd.print(choices[1]);
      lcd.setCursor(0, Selection);
      updateLCD = false;      
    }
  }
  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce); 
    
  return Selection;
}

float FloatMenu(String message, String units, float choices[], float currentChoice) {
            
  updateLCD = true;
  while (digitalRead(enterButton) == LOW) {
   currentStateA = digitalRead(CLK);
    if (currentStateA != lastStateA  && currentStateA == 1){
      if (digitalRead(DT) == currentStateA) { // Clockwise
        currentChoice += choices[1];
        if (currentChoice > choices[2]){
          currentChoice = choices[0];
        }
      } else { // Counterclockwise
        currentChoice -= choices[1];
        if (currentChoice < choices[0]){
          currentChoice = choices[2];
        }
      }
      updateLCD = true;
    }
    lastStateA = currentStateA;
    delay(1);

    if (updateLCD == true) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(message);
      lcd.setCursor(0, 1);
      lcd.print((String) currentChoice + units);        
      lcd.setCursor(0, 1);
      updateLCD = false;      
    }
  }

  while (digitalRead(enterButton) == HIGH) {}
  delay(debounce); 
  
  return currentChoice;
}
