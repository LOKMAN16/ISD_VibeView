//Serial setup
char serialInput[6];

//PWM setup
const unsigned int left_vib = 6, left_led = 5, right_vib = 9, right_led = 3;
unsigned int intensL = 0, freqL = 0, intensR = 0, freqR = 0;
unsigned long timePassed = 0;
bool isNotVisible = 0, isCorrectDir = 0;

void setup() {
  Serial.begin(115200);

  pinMode(left_vib, OUTPUT);
  pinMode(left_led, OUTPUT);
  pinMode(right_vib, OUTPUT);
  pinMode(right_led, OUTPUT);

  analogWrite(left_vib, 120);
  delay(500);
  analogWrite(left_vib, 0);
  analogWrite(right_vib, 120);
  delay(500);
  analogWrite(left_vib, 0);
}

void loop() {

  if (Serial.available()) {
    serialInput[0] = Serial.read();
    serialInput[1] = Serial.read();
    serialInput[2] = Serial.read();
    serialInput[3] = Serial.read();
    serialInput[4] = Serial.read();
    serialInput[5] = Serial.read();
  }

  timePassed = millis();

  //serialInput[0]
  switch (serialInput[0]) {
    case 0:
      {
        serialInput[5] == 3 ? intensL = 0 : intensL = 60;
      }
      break;
    case 1: intensL = 105; break;
    case 2: intensL = 150; break;
    case 3: intensL = 195; break;
    case 4: intensL = 240; break;
    case 5: intensL = 240; break;
  }

  //serialInput[1]
  switch (serialInput[1]) {
    case 0:
      {
        serialInput[5] == 3 ? intensR = 0 : intensR = 60;
      }
      break;
    case 1: intensR = 105; break;
    case 2: intensR = 150; break;
    case 3: intensR = 195; break;
    case 4: intensR = 240; break;
    case 5: intensR = 240; break;
  }

  //serialInput[2]
  switch (serialInput[2]) {
    case 0:
    case 1:
      freqL = 1000;
      freqR = 1000;
      break;
    case 2:
      freqL = 750;
      freqR = 750;
      break;
    case 3:
      freqL = 500;
      freqR = 500;
      break;
    case 4:
      freqL = 250;
      freqR = 250;
      break;
    case 5:
      freqL = 250;
      freqR = 250;
      break;
  }

  //serialInput[5]
  switch (serialInput[5]) {
    case 0:
      {
        isNotVisible = 0;
        isCorrectDir = 0;
      }
      break;
    case 1: goForward(); break;
    case 2: reachable(); break;
    case 3: notVisible(); break;
    case 4: readyToGo(); break;
    case 5: correctDir(); break;
  }

  leftVib(intensL, freqL);
  rightVib(intensR, freqR);
}

void leftVib(int intensL, int freqL) {


  if (freqL == 0) {
    analogWrite(left_vib, intensL);
    analogWrite(left_led, intensL);
  } else {
    if (((timePassed / freqL) % 2) == 0) {
      analogWrite(left_vib, intensL);
      analogWrite(left_led, intensL);
    } else {
      analogWrite(left_vib, 0);
      analogWrite(left_led, 0);
    }
  }
}

void rightVib(int intensR, int freqR) {
  if (freqR == 0) {
    analogWrite(right_vib, intensR);
    analogWrite(right_led, intensR);
  } else {
    if (((timePassed / freqR) % 2) == 0) {
      analogWrite(right_vib, intensR);
      analogWrite(right_led, intensR);
    } else {
      analogWrite(right_vib, 0);
      analogWrite(right_led, 0);
    }
  }
}

void goForward() {}

void reachable() {
  leftVib(150, 0);
  delay(250);
  leftVib(0, 0);
  rightVib(150, 0);
  delay(250);
  rightVib(0, 0);
}

void notVisible() {
  /*
  if (isNotVisible == 0) {
    leftVib(240, 0);
    rightVib(240, 0);
    delay(500);
    leftVib(0, 0);
    rightVib(0, 0);
    delay(250);
    leftVib(240, 0);
    rightVib(240, 0);
    delay(250);
    leftVib(0, 0);
    rightVib(0, 0);
    delay(250);
    isNotVisible = 1;
  }
  */
}

void readyToGo() {
  /*
  //allVib(120, 0);
  delay(500);
  //allVib(0, 0);
  delay(250);
  //allVib(120, 0);
  delay(250);
  //allVib(0, 0);
  delay(250);
  //allVib(120, 0);
  delay(500);
  //allVib(0, 0);
  */
}

void correctDir() {
  if (isCorrectDir == 0) {
    leftVib(120, 0);
    rightVib(120, 0);
    delay(250);
    leftVib(0, 0);
    rightVib(0, 0);
    delay(250);
    leftVib(120, 0);
    rightVib(120, 0);
    delay(250);
    leftVib(0, 0);
    rightVib(0, 0);
    delay(250);
    isCorrectDir = 1;
  }
}