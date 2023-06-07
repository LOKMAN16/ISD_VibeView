
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
char serialInput[7];
char previousInput[2];

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
  if (Serial.available() >= 7) { // Make sure at least 7 bytes are available
    serialInput[0] = Serial.read();
    serialInput[1] = Serial.read();
    serialInput[2] = Serial.read();
    serialInput[3] = Serial.read();
    serialInput[4] = Serial.read();
    serialInput[5] = Serial.read();
    serialInput[6] = Serial.read();
  }

  // Update interval1 and interval2 with new values
  int duration = (serialInput[3] - '0') * 10 + (serialInput[4] - '0');
 // Combine digits to get duration value
  unsigned long interval1 = duration * 100;
  unsigned long interval2 = interval1 / 2;
  Serial.println(serialInput[0]);
  Serial.println(serialInput[1]);
  Serial.println(serialInput[2]);
  Serial.println(serialInput[3]);
  Serial.println(serialInput[4]);
  Serial.println(serialInput[5]);
  Serial.println(serialInput[6]);
  switch(serialInput[0]){
    case '0':LAxis(0);break;
    case '1':LAxis(60);break;
    case '2':LAxis(105);break;
    case '3':LAxis(150);break;
    case '4':LAxis(195);break;
    case '5':LAxis(240);break;
    default:
          break;
  }
  switch(serialInput[1]){
    case '0':RAxis(0);break;
    case '1':RAxis(60);break;
    case '2':RAxis(105);break;
    case '3':RAxis(150);break;
    case '4':RAxis(195);break;
    case '5':RAxis(240);break;
    default:
          break;
  }
  
  switch(serialInput[2]){
    case '0':break;
    case '1':fdistance(&duration, &interval1, &interval2);break;
    case '2':fdistance(&duration, &interval1, &interval2);break;
    case '3':fdistance(&duration, &interval1, &interval2);break;
    case '4':fdistance(&duration, &interval1, &interval2);break;
    case '5':fdistance(&duration, &interval1, &interval2);break;
    case '6':fdistance(&duration, &interval1, &interval2);break;
    default:
          break;
  }

  switch(serialInput[5]){
    case '0':break;
    case '1':fdistance(&duration, &interval1, &interval2);break;
    case '2':Reachable();break;
    case '3':Notvisible();break;
    case '4':Readygo();break;
    case '5':Correctdirection();break;
    default:
          break;
  }
  
  

}
int Transfer(char ch1, char ch2){
  int num = int(ch1 + ch2);
  return abs(num);
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

void LAxis(int Xdegree) {
  bool exit = false;
  
  Serial.println("LAxis");
  

  while (Xdegree != 0 && (serialInput[0] == '0' || serialInput[1] == '0')) {
    Serial.println("WLAxis");
    Serial.println(Xdegree);
    // Update the Udegree value if there is new input from the Serial Monitor
    
     if (Xdegree > 0) {
      analogWrite(motor3Pin, 0);
      analogWrite(motor2Pin, Xdegree);
      return;
    }
    
    
    // Perform the motor activation
    
  }
  if (Xdegree == 0) {
      // Exit the function if the 'X' command was entered
      analogWrite(motor2Pin, 0);
      return;
    }
}
void RAxis(int Xdegree) {
  bool exit = false;
  
  Serial.println("RAxis");
  

  while (Xdegree != 0 && (serialInput[0] == '0' || serialInput[1] == '0')) {
    Serial.println("WRAxis");
    Serial.println(Xdegree);
    // Update the Udegree value if there is new input from the Serial Monitor
    
    if (Xdegree > 0) {
      // int Deg =abs(Xdegree);
      analogWrite(motor2Pin, 0);
      analogWrite(motor3Pin, Xdegree);
      return;
    } 
    
  }
  if (Xdegree == 0) {
      // Exit the function if the 'X' command was entered
      analogWrite(motor3Pin, 0);
      return;
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

void Reachable() {
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
        serialInput[5]='0';
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
  Serial.println("Correctdirection");
  for (int i = 0; i < 4; i++) {
      //Serial.println(durations[i%4]);
      //Serial.println(i%2+4);
      analogWrite(motor1Pin, dutyCycles[i%2+4][0]);
      analogWrite(motor2Pin, dutyCycles[i%2+4][1]);
      analogWrite(motor3Pin, dutyCycles[i%2+4][2]);
      analogWrite(motor4Pin, dutyCycles[i%2+4][3]);
      delay(durations[i%4]);
  }stop();
  serialInput[5]='0';
  
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
  serialInput[5]='0';
  
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
    serialInput[5]='0';


}


void fdistance(int* duration, unsigned long* interval1, unsigned long* interval2) {
  unsigned long previousMillis = 0;
  Serial.println("frequency distance");

  while (true) {
    // check for new input from the Serial Monitor
    if (Serial.available() >= 7) {
      serialInput[0] = Serial.read();
      serialInput[1] = Serial.read();
      serialInput[2] = Serial.read();
      serialInput[3] = Serial.read();
      serialInput[4] = Serial.read();
      serialInput[5] = Serial.read();
      serialInput[6] = Serial.read();
      
      // Update duration based on new input
      *duration = (serialInput[3] - '0') * 10 + (serialInput[4] - '0');
      
      // exit the function if the 6th digit is not equal to '1'
      if (serialInput[5] != '1') {
        stop();
        Serial.println("EXIT");
        return;
      }
      
      // update interval1 and interval2 based on new duration value
      *interval1 = *duration * 100;
      *interval2 = *interval1 / 2;
    }

    // perform the motor activation
    analogWrite(motor2Pin, Uits);
    analogWrite(motor3Pin, Uits);
    delay(*interval2);
    analogWrite(motor2Pin, 0);
    analogWrite(motor3Pin, 0);
    delay(*interval1 - *interval2);
  }
}

