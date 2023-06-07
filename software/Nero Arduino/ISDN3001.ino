
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



void setup() {
  // Initialize the motor pins as outputs
  Serial.begin(9600);
  pinMode(motor1Pin, OUTPUT);
  pinMode(motor2Pin, OUTPUT);
  pinMode(motor3Pin, OUTPUT);
  pinMode(motor4Pin, OUTPUT);
  
}

void loop() {
  while (true) {
    if (Serial.available() > 0) {
      char PG=Serial.read();
      int intVal = Serial.parseInt();
      switch (PG) {
        // case '1': motor1();   break;
        // case '2': motor2();   break;
        // case '3': motor3();   break;
        // case '4': motor4();   break;
        case 'L': left(intVal);     break;
        case 'R': right(intVal);    break;
        case 'U': up(intVal);       break;
        case 'D': down(intVal);     break;
        case 'l': fleft(intVal);     break;
        case 'r': fright(intVal);    break;
        case 'u': fup(intVal);       break;
        case 'd': fdown(intVal);     break;
        case 'I': ITS();      break;
        case 'i': LITS();      break;
        case 'A': RotationCLW(); break;
        case 'B': RotationCCLW(); break;
        case 'C': Notvisible();  break;
        case 'E': Correctdirection();  break;
        case 'F': Readygo();  break;
        case 'G': Distance(intVal);  break;
        case 'g': fdistance(intVal);  break;
        case 'S': stop();      break;
        default:
          Serial.println("Menu Invalid command");
          break;
      }
      // consume any remaining characters in the input buffer, including newline characters
      while (Serial.available() > 0) {
        Serial.read();
      }
    }
  }

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

void wait(unsigned long duration) {
  unsigned long startTime = millis(); // get the current time value
  unsigned long elapsedTime = 0; // initialize the elapsed time to zero

  while (elapsedTime < duration) {
    // do something while waiting, such as update the motor duty cycle
    // or check for new input from the Serial Monitor
    
    elapsedTime = millis() - startTime; // calculate the elapsed time
  }
}

void down(int Ddegree) {
  bool exit = false;
  Serial.println("IntDown");

  while (!exit) {
    // update the Udegree value if there is new input from the Serial Monitor
    updatedegree(Ddegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }
    // perform the motor activation
    analogWrite(motor1Pin, Ddegree);
    Serial.println(Ddegree);
    wait(Ufqy);

  }
}

void right(int Rdegree) {
  bool exit = false;
  Serial.println("IntRight");

  while (!exit) {
    // update the Udegree value if there is new input from the Serial Monitor
    updatedegree(Rdegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }
    // perform the motor activation
    analogWrite(motor2Pin, 240);
    analogWrite(motor3Pin, Rdegree);
    Serial.println(Rdegree);
    wait(Ufqy);

  }
}

void left(int Ldegree) {
  bool exit = false;
  Serial.println("IntLeft");

  while (!exit) {
    // update the Udegree value if there is new input from the Serial Monitor
    updatedegree(Ldegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }
    // perform the motor activation
    analogWrite(motor3Pin, 240);
    analogWrite(motor2Pin, Ldegree);
    Serial.println(Ldegree);
    wait(Ufqy);

  }
}
// void left(int Ldegree){
//   analogWrite(motor3Pin, Ldegree); // Set motor2 to 75% duty cycle
//   delay(Ufqy); // Wait for a short period of time
//   // analogWrite(motor3Pin, 0); // Turn off motor2
//   // delay(2000);
// }
void up(int Udegree) {
  bool exit = false;
  Serial.println("Intup");

  while (!exit) {
    // update the Udegree value if there is new input from the Serial Monitor
    updatedegree(Udegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }
    // perform the motor activation
    analogWrite(motor4Pin, Udegree);
    Serial.println(Udegree);
    wait(Ufqy);

  }
}
// void up(int Udegree){
//   analogWrite(motor4Pin, Udegree); // Set motor2 to 75% duty cycle
//   delay(Ufqy); // Wait for a short period of time
//   // analogWrite(motor4Pin, 0); // Turn off motor2
//   // delay(2000);
// }
void fdown(int Ddegree) {
  unsigned long previousMillis = 0;
  unsigned long interval1 = Ddegree * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println("frequency down");

  while (true) {
    // check for new input from the Serial Monitor
    bool exit = false;
    updatedegree(Ddegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }

    // update the interval1 and interval2 variables with the new Ddegree value
    interval1 = Ddegree * 100;
    interval2 = interval1 / 2;

    // perform the motor activation
    analogWrite(motor4Pin, 255);
    wait(interval2);
    analogWrite(motor4Pin, 0);
    wait(interval1 - interval2);
  }
}
void fright(int Rdegree) {
  unsigned long previousMillis = 0;
  unsigned long interval1 = Rdegree * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println("frequency right");

  while (true) {
    // check for new input from the Serial Monitor
    bool exit = false;
    updatedegree(Rdegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }

    // update the interval1 and interval2 variables with the new Rdegree value
    interval1 = Rdegree * 100;
    interval2 = interval1 / 2;

    // perform the motor activation
    analogWrite(motor2Pin, Uits);
    wait(interval2);
    analogWrite(motor2Pin, 0);
    wait(interval1 - interval2);
  }
}
void fleft(int Ldegree) {
  unsigned long previousMillis = 0;
  unsigned long interval1 = Ldegree * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println("frequency left");

  while (true) {
    // check for new input from the Serial Monitor
    bool exit = false;
    updatedegree(Ldegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }

    // update the interval1 and interval2 variables with the new Ldegree value
    interval1 = Ldegree * 100;
    interval2 = interval1 / 2;

    // perform the motor activation
    analogWrite(motor3Pin, Uits);
    wait(interval2);
    analogWrite(motor3Pin, 0);
    wait(interval1 - interval2);

  }
}
void fup(int Udegree) {
  unsigned long previousMillis = 0;
  unsigned long interval1 = Udegree * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println("frequency up");

  while (true) {
    // check for new input from the Serial Monitor
    bool exit = false;
    updatedegree(Udegree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }

    // update the interval1 and interval2 variables with the new Udegree value
    interval1 = Udegree * 100;
    interval2 = interval1 / 2;

    // perform the motor activation
    analogWrite(motor4Pin, Uits);
    wait(interval2);
    analogWrite(motor4Pin, 0);
    wait(interval1 - interval2);

    
  }
}

// void fup(int Udegree){
//   while (!Serial.available()){
//   analogWrite(motor4Pin, 255); // Set motor2 to 75% duty cycle
//   delay(Udegree*100); // Wait for a short period of time
//   analogWrite(motor4Pin, 0); // Turn off motor2
//   delay(Udegree*100);
//   };
// }
void stop(){
  Serial.println("stop");
  analogWrite(motor1Pin,0);
  analogWrite(motor2Pin,0);
  analogWrite(motor3Pin,0);
  analogWrite(motor4Pin,0);
}
// // Function to control motor 1
// void left() {
//   Serial.println("left");
//   // Set the PWM duty cycle for motor 1
//   analogWrite(motor3Pin, Uits);
//   analogWrite(motor4Pin, Uits); 
//   delay(Ufqy); // Wait for a short period of time
//   analogWrite(motor3Pin, 0);
//   analogWrite(motor4Pin, 0); // Turn off motor1
//   delay(2000); // Wait for a short period of time
// }

// // Function to control motor 2
// void right() {
//   Serial.println("right");
//   // Set the PWM duty cycle for motor 2
//   analogWrite(motor1Pin, Uits); // Set motor2 to 75% duty cycle
//   analogWrite(motor2Pin, Uits);
//   delay(Ufqy); // Wait for a short period of time
//   analogWrite(motor1Pin, 0);
//   analogWrite(motor2Pin, 0); // Turn off motor2
//   delay(2000); // Wait for a short period of time
// }

// // Function to control motor 3
// void up() {
//   Serial.println("Up");
//   // Set the PWM duty cycle for motor 3
//   analogWrite(motor2Pin, Uits);
//   analogWrite(motor4Pin, Uits); 
//   delay(Ufqy); // Wait for a short period of time
//   analogWrite(motor2Pin, 0);
//   analogWrite(motor4Pin, 0); // Turn off motor3
//   delay(2000); // Wait for a short period of time
// }

// // Function to control motor 4
// void down() {
//   Serial.println("Down");
//   // Set the PWM duty cycle for motor 4
//   analogWrite(motor1Pin, Uits);
//   analogWrite(motor3Pin, Uits); // Set motor4 to 100% duty cycle
//   delay(Ufqy); // Wait for a short period of time
//   analogWrite(motor1Pin, 0);
//   analogWrite(motor3Pin, 0); // Turn off motor4
//   delay(2000); // Wait for a short period of time
// }
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
// Function to create a custom vibration pattern
// void vibrateWithPattern() {
//   Serial.println("Pattern test Options input ");
  
    // Define an array of duration values in milliseconds
    // int durations[] = {75, 100};
    
    // // Define an array of duty cycle values for each motor
    // int dutyCycles[][4] = {
    //   {255, 0, 0, 0}, // Vibrate motor 1 
    //   {0, 255, 0, 0}, // Vibrate motor 2
    //   {0, 0, 0, 255}, 
    //   {0, 0, 255, 0} 
    // };

    

//   if (Serial.available()>0)
//   { 
//       char PatInput=Serial.read();  
//       if (PatInput == 'A'){
//       // Loop through the array of duration values and vibrate each motor for the specified duration
//         RotationCLW();
      
//       }else if (PatInput == 'B'){
//         //Serial.println(i);
//          RotationCCLW();
//         }
//   }
// }
void RotationCLW() {
          //int durations[] = {75,75, 250};
      Serial.println("rotate clockwise");
          // Define an array of duty cycle values for each motor
          
          for (int i = 0; i < 8; i++) {
            //Serial.println(durations[(i/4)%3]);
            for (int j = 0; j < 4; j++) {
              analogWrite(motor1Pin, dutyCycles[i%4][0]);
              analogWrite(motor2Pin, dutyCycles[i%4][1]);
              analogWrite(motor3Pin, dutyCycles[i%4][2]);
              analogWrite(motor4Pin, dutyCycles[i%4][3]);
              delay(75);//(durations[(i/4)%3]);// 50
              }
          }
        stop();
}
      
void RotationCCLW() {
  Serial.println("rotate counterclockwise");
        for (int i = 0; i < 36; i++) {
              //Serial.println(i);
              for (int j = 0; j < 4; j++) {
                analogWrite(motor1Pin, dutyCycles[i%4][3]);
                analogWrite(motor2Pin, dutyCycles[i%4][2]);
                analogWrite(motor3Pin, dutyCycles[i%4][1]);
                analogWrite(motor4Pin, dutyCycles[i%4][0]);
                delay(75);//(durations[i]); 50
                }
        }
        stop();
}

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
// void Distance(int IntDis, int CurDis) {
//   int Ipattern[][6] = {
//       {240,240,240,240}, // Vibrate motor 1 
//       {180,180,180,180}, // Vibrate motor 2
//       {120, 120, 120, 120}, 
//       {60, 60, 60, 60}, 
//       {0,0,0,0}
//       };
//   int i=0;

//   switch(CurDis /(IntDis/6)){
//     case 6: i=4; break;
//     case 5: i=4; break;
//     case 4: i=3; break;
//     case 3: i=2; break;
//     case 2: i=1; break;
//     case 1: i=0; break;
//     default:
//         Serial.println("Error distance");
//         break;
//     }
    
//       analogWrite(motor1Pin, Ipattern[i][3]);
//       analogWrite(motor2Pin, Ipattern[i][2]);
//       analogWrite(motor3Pin, Ipattern[i][1]);
//       analogWrite(motor4Pin, Ipattern[i][0]);
//       delay(75);
    
  
// }
void Distance(int IntDis) {
  int Ipattern[][4] = {
      {240, 240, 240, 240}, // Vibrate motor 1
      {180, 180, 180, 180}, // Vibrate motor 2
      {120, 120, 120, 120},
      {60, 60, 60, 60},
      {0, 0, 0, 0}
  };
  int i = 0;
  int CurDis = IntDis;
  Serial.println("Distance");
  Serial.println(CurDis);
  
  while (true) {
    // check for new input from the Serial Monitor
    bool exit = false;
    updatedegree(CurDis, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }

  switch (CurDis / (IntDis / 6)) {
    case 6: 
    case 5: i=4; break;
    case 4: i=3; break;
    case 3: i=2; break;
    case 2: i=1; break;
    case 1: i=0; break;
    default:
      Serial.println("Error distance");
      break;
  }

  analogWrite(motor1Pin, Ipattern[i][3]);
  analogWrite(motor2Pin, Ipattern[i][2]);
  analogWrite(motor3Pin, Ipattern[i][1]);
  analogWrite(motor4Pin, Ipattern[i][0]);
  wait(75);
  }

  // check for new input from the Serial Monitor
  
}
void fdistance(int degree) {
  unsigned long previousMillis = 0;
  unsigned long interval1 = degree * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println("frequency distance");

  while (true) {
    // check for new input from the Serial Monitor
    bool exit = false;
    updatedegree(degree, exit);
    if (exit) {
      // exit the function if the 'X' command was entered
      stop();
      Serial.println("EXIT");
      return;
    }

    // update the interval1 and interval2 variables with the new Ddegree value
    interval1 = degree * 100;
    interval2 = interval1 / 2;

    // perform the motor activation
    analogWrite(motor1Pin, Uits);
    analogWrite(motor2Pin, Uits);
    analogWrite(motor3Pin, Uits);
    analogWrite(motor4Pin, Uits);
    wait(interval2);
    analogWrite(motor1Pin, 0);
    analogWrite(motor2Pin, 0);
    analogWrite(motor3Pin, 0);
    analogWrite(motor4Pin, 0);
    wait(interval1 - interval2);
  }
}
