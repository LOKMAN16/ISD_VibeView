
// Define the motor pins
int motor1Pin = 3;
int motor2Pin = 5;
int motor3Pin = 6;
int motor4Pin = 9;
int dutyCycles[][6] = {
     {255, 0, 0, 0}, // Vibrate motor 1 
      {0, 255, 0, 0}, // Vibrate motor 2
      {0, 0, 0, 255}, 
      {0, 0, 255, 0}, 
      {255,255,255,255},//ON
      {0,0,0,0}//off
     };
int distance = 3000;
int Uits = 240 ;
int Ufqy = 1000;
int Ustop = 1000;
unsigned long interval1 = 0;
unsigned long interval2 = 0;
char serialInput[6];

//arduino diem 43 45
//  19

void setup() {
  // Initialize the motor pins as outputs
  Serial.begin(9600);
  pinMode(motor1Pin, OUTPUT);
  pinMode(motor2Pin, OUTPUT);
  pinMode(motor3Pin, OUTPUT);
  pinMode(motor4Pin, OUTPUT);
  
//   for (int i = 0; i < 70; i++){
//   XAxis(i);}
 }

void loop() {
  if (Serial.available()){
    serialInput[0] = Serial.read();
    serialInput[1] = Serial.read();
    serialInput[2] = Serial.read();
    serialInput[3] = Serial.read();
    serialInput[4] = Serial.read();
    serialInput[5] = Serial.read();
  }
  Serial.println("X");
  Serial.println(serialInput[0]);
  Serial.println(serialInput[1]);
  Serial.println(serialInput[2]);
  Serial.println(serialInput[3]);
  Serial.println(serialInput[4]);
  Serial.println("X");
  // switch(serialInput[0]){case '0':XAxis}
  // while (true) {
  //   if (Serial.available() > 0) {
  //     char PG=Serial.read();
  //     int intVal = Serial.parseInt();
  //     switch (PG) {
  //       // case '1': motor1();   break;
  //       // case '2': motor2();   break;
  //       // case '3': motor3();   break;
  //       // case '4': motor4();   break;
  //       // case 'L': left(intVal);     break;
  //       case 'H': XAxis(intVal);    break;
  //       case 'h': FXAxis(intVal,15);    break;
  //       // case 'U': up(intVal);       break;
  //       // case 'D': down(intVal);     break;
  //       // case 'l': fleft(intVal);     break;
  //       // case 'r': fright(intVal);    break;
  //       // case 'u': fup(intVal);       break;
  //       // case 'd': fdown(intVal);     break;
  //       // case 'I': ITS();      break;
  //       // case 'i': LITS();      break;
  //       case 'R': Reactable(); break;
  //       case 'B': RotationCCLW(); break;
  //       case 'N': Notvisible();  break;
  //       case 'C': Correctdirection();  break;
  //       case 'G': Readygo();  break;
  //       case 'P': Distance(intVal);  break;
  //       case 'D': fdistance(intVal);  break;
  //       case 'S': stop();      break;
  //       default:
  //         Serial.println("Menu Invalid command");
  //         break;
  //     }
  //     // consume any remaining characters in the input buffer, including newline characters
  //     while (Serial.available() > 0) {
  //       Serial.read();
  //     }
  //   }
  // }

}
void exitFunction() {
  // check for the 'X' command to exit the function
  while (true) {
    if (Serial.available() > 0) {
      char cmd = Serial.read();
      if (cmd == 'X') {
        stop(); 
        Serial.println("EXIT");// stop the motor
        return; // exit the function and return to the loop()
      }
    }
  }
}
void updatedegree(int &Udegree, bool &exit) {
  // check for new input from the Serial Monitor
  if (Serial.available() > 0) {
    String inputString = Serial.readStringUntil('\n');
    inputString.trim();
    if (inputString == "X") {
      exit = true;
      return;
    }
    int intVal = inputString.toInt();
    if (intVal == 0) {
      Serial.println("Invalid input value");
    } else {
      // update the Udegree variable with the new intensity value
      Udegree = intVal;
    }
  }
}
void updateb(int &degree, int &duration, bool &exit) {
  // check for new input from the Serial Monitor
  if (Serial.available() > 0) {
    // read the first input value
    String inputString1 = Serial.readStringUntil('\n');
    inputString1.trim();
    if (inputString1 == "X") {
      exit = true;
      return;
    }
    int intVal1 = inputString1.toInt();
    if (intVal1 == 0) {
      Serial.println("Invalid input value");
    } else {
      // update the Udegree1 variable with the new input value
      degree = intVal1;
    }

    // read the second input value
    if (Serial.available() > 0) {
      String inputString2 = Serial.readStringUntil('\n');
      inputString2.trim();
      if (inputString2 == "X") {
        exit = true;
        return;
      }
      int intVal2 = inputString2.toInt();
      if (intVal2 == 0) {
        Serial.println("Invalid input value");
      } else {
        // update the Udegree2 variable with the new input value
        degree = intVal1;
        duration = intVal2;
      }
    }
    Serial.println("Updated input value");
  }
}

void wait(unsigned long duration) {
  unsigned long startTime = millis(); // get the current time value
  unsigned long elapsedTime = 0; // initialize the elapsed time to zero

  while (elapsedTime < duration) {
    // do something while waiting, such as update the motor duty cycle
    // or check for new input from the Serial Monitor
    
    elapsedTime = millis() - startTime; // calculate the elapsed time
  }
}
int conversion(int degree){
  if(degree<6 || degree== 0){
    return 0;
  }else if(degree>=6 && degree < 15){return 60;
  }else if(degree>=15 && degree < 30){return 105;
  }else if(degree>=30 && degree < 45){return 150;
  }else if(degree>=45 && degree < 60){return 195;
  }else if(degree>=60){return 240;}
}

void FXAxis(int Xdegree, int duration) {
  bool exit = false;
  unsigned long previousMillis = 0;
  unsigned long interval1 = duration * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println("XAxis");

  while (!exit) {
    // update the Udegree value if there is new input from the Serial Monitor
    updateb(Xdegree, duration, exit);

    interval1 = duration * 100;
    interval2 = interval1 / 2;

    if (Xdegree<0){
      int Deg =abs(Xdegree);
      analogWrite(motor2Pin, 60);
      analogWrite(motor3Pin, conversion(Deg));
      wait(interval2);
      analogWrite(motor2Pin, 0);
      analogWrite(motor3Pin, 0);
      wait(interval1 - interval2);
    } else if (Xdegree>=0){
      analogWrite(motor3Pin, 60);
      analogWrite(motor2Pin, conversion(Xdegree));
      wait(interval2);
      analogWrite(motor2Pin, 0);
      analogWrite(motor3Pin, 0);
      wait(interval1 - interval2);
      }
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }
    // perform the motor activation
    Serial.println(Xdegree);
    Serial.println(duration);
  }
}

void XAxis(int Xdegree) {
  bool exit = false;
  
  Serial.println("XAxis");

  while (!exit) {
    // update the Udegree value if there is new input from the Serial Monitor
    updatedegree(Xdegree, exit);
    if (Xdegree<0){
      int Deg =abs(Xdegree);
      analogWrite(motor2Pin,0);
      analogWrite(motor3Pin, conversion(Deg));
    } else if (Xdegree>=0){
      analogWrite(motor3Pin,0);
      analogWrite(motor2Pin, conversion(Xdegree));
      }
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }
    // perform the motor activation
    Serial.println(Xdegree);
  }
}










void stop(){
  Serial.println("stop");
  analogWrite(motor1Pin,0);
  analogWrite(motor2Pin,0);
  analogWrite(motor3Pin,0);
  analogWrite(motor4Pin,0);
}

void ITS() {
  Serial.println("intensity test");
  int x = 60;
  do {Serial.println(x);
    analogWrite(motor1Pin,x);
    analogWrite(motor2Pin,x);
    analogWrite(motor3Pin,x);
    analogWrite(motor4Pin,x);
    delay(1000);
    x=x + 60;

  }while (x < 255);
  analogWrite(motor1Pin,0);
  analogWrite(motor2Pin,0);
  analogWrite(motor3Pin,0);
  analogWrite(motor4Pin,0);
}
void LITS() {
  Serial.println("intensity test");
  int x = 240;
  do {Serial.println(x);
    // analogWrite(motor1Pin,x);
    // analogWrite(motor2Pin,x);
    analogWrite(motor3Pin,x);
    //analogWrite(motor4Pin,x);
    delay(1000);
    //analogWrite(motor3Pin,0);
    //delay(100);
    x=x - 60;

  }while (x > 50);
  // analogWrite(motor1Pin,0);
  // analogWrite(motor2Pin,0);
  analogWrite(motor3Pin,0);
  // analogWrite(motor4Pin,0);
}

void Reactable() {
          //int durations[] = {75,75, 250};
      Serial.println("Reactable");
          // Define an array of duty cycle values for each motor
          
          for (int i = 0; i < 12; i++) {
            //Serial.println(durations[(i/4)%3]);
            for (int j = 0; j < 2; j++) {
              analogWrite(motor2Pin, dutyCycles[i%2][1]);
              analogWrite(motor3Pin, dutyCycles[i%2][2]);
              delay(75);//(durations[(i/4)%3]);// 50
              }
          }
        stop();
}
      
// void RotationCCLW() {
//   Serial.println("rotate counterclockwise");
//         for (int i = 0; i < 36; i++) {
//               //Serial.println(i);
//               for (int j = 0; j < 4; j++) {
//                 analogWrite(motor1Pin, dutyCycles[i%4][3]);
//                 analogWrite(motor2Pin, dutyCycles[i%4][2]);
//                 analogWrite(motor3Pin, dutyCycles[i%4][1]);
//                 analogWrite(motor4Pin, dutyCycles[i%4][0]);
//                 delay(75);//(durations[i]); 50
//                 }
//         }
//         stop();
// }

void Pattern1() {
  int durations[] = {75,200,75,500,300,750};
  Serial.println("Pattern 1");
  for (int i = 0; i < 80; i++) {
      //Serial.println(durations[i%4]);
      //Serial.println(i%2+4);
      analogWrite(motor1Pin, dutyCycles[i%2+4][0]);
      analogWrite(motor2Pin, dutyCycles[i%2+4][1]);
      analogWrite(motor3Pin, dutyCycles[i%2+4][2]);
      analogWrite(motor4Pin, dutyCycles[i%2+4][3]);
      delay(durations[i%6]);
  }stop();
  
}
void Correctdirection() {
  int durations[] = {100,200,100,500};
  Serial.println("Pattern 1");
  for (int i = 0; i < 4; i++) {
      //Serial.println(durations[i%4]);
      //Serial.println(i%2+4);
      analogWrite(motor1Pin, dutyCycles[i%2+4][0]);
      analogWrite(motor2Pin, dutyCycles[i%2+4][1]);
      analogWrite(motor3Pin, dutyCycles[i%2+4][2]);
      analogWrite(motor4Pin, dutyCycles[i%2+4][3]);
      delay(durations[i%4]);
  }stop();
  
}
void Notvisible() {
  int durations[] = {400,200,150,500};
  Serial.println("Not visible");
  for (int i = 0; i < 4; i++) {
      //Serial.println(durations[i%4]);
      //Serial.println(i%2+4);
      analogWrite(motor1Pin, dutyCycles[i%2+4][0]);
      analogWrite(motor2Pin, dutyCycles[i%2+4][1]);
      analogWrite(motor3Pin, dutyCycles[i%2+4][2]);
      analogWrite(motor4Pin, dutyCycles[i%2+4][3]);
      delay(durations[i%4]);
  }stop();
  
}
void Readygo() {
  Serial.println("Ready to go");
  int x = 240;
  int durations[] = {800,100,400,100,400,100,400,100,400};
  do {
    //Serial.println(x);
    analogWrite(motor1Pin,x);
    analogWrite(motor2Pin,x);
    analogWrite(motor3Pin,x);
    analogWrite(motor4Pin,x);
    delay(durations[x/60*2]);
    //Serial.println(durations[x/60*2]);
    analogWrite(motor1Pin,0);
    analogWrite(motor2Pin,0);
    analogWrite(motor3Pin,0);
    analogWrite(motor4Pin,0);
    delay(durations[x/60*2-1]);
    //analogWrite(motor3Pin,0);
    //delay(100);
    x=x - 60;

  }while (x > 50);
    delay(200);
    analogWrite(motor1Pin,250);
    analogWrite(motor2Pin,250);
    analogWrite(motor3Pin,250);
    analogWrite(motor4Pin,250);
    delay(durations[0]);
    analogWrite(motor1Pin,0);
    analogWrite(motor2Pin,0);
    analogWrite(motor3Pin,0);
    analogWrite(motor4Pin,0);
    delay(100);


}

// void Distance(int IntDis) {
//   int Ipattern[][4] = {
//       {240, 240, 240, 240}, // Vibrate motor 1
//       {180, 180, 180, 180}, // Vibrate motor 2
//       {120, 120, 120, 120},
//       {60, 60, 60, 60},
//       {0, 0, 0, 0}
//   };
//   int i = 0;
//   int CurDis = IntDis;
//   Serial.println("Distance");
//   Serial.println(CurDis);
  
//   while (true) {
//     // check for new input from the Serial Monitor
//     bool exit = false;
//     updatedegree(CurDis, exit);
//     if (exit) {
//       // exit the function if the 'X' command was entered
//       stop();
//       Serial.println("EXIT");
//       return;
//     }

//   switch (CurDis / (IntDis / 6)) {
//     case 6: 
//     case 5: i=4; break;
//     case 4: i=3; break;
//     case 3: i=2; break;
//     case 2: i=1; break;
//     case 1: i=0; break;
//     default:
//       Serial.println("Error distance");
//       break;
//   }

//   analogWrite(motor1Pin, Ipattern[i][3]);
//   analogWrite(motor2Pin, Ipattern[i][2]);
//   analogWrite(motor3Pin, Ipattern[i][1]);
//   analogWrite(motor4Pin, Ipattern[i][0]);
//   wait(75);
//   }

//   // check for new input from the Serial Monitor
  
// }
void fdistance(int duration) {
  unsigned long previousMillis = 0;
  unsigned long interval1 = duration * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println("frequency distance");

  while (true) {
    // check for new input from the Serial Monitor
    bool exit = false;
    updatedegree(duration, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }

    // update the interval1 and interval2 variables with the new Ddegree value
    interval1 = duration * 100;
    interval2 = interval1 / 2;

    // perform the motor activation
    analogWrite(motor2Pin, Uits);
    analogWrite(motor3Pin, Uits);
    wait(interval2);
    analogWrite(motor2Pin, 0);
    analogWrite(motor3Pin, 0);
    wait(interval1 - interval2);
  }
}
