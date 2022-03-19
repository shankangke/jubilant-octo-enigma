#define MAX_SECONDS 1
#define DETECT_PERIOD 5
#define BACK_MILLIS 400
#define GO_MILLIS 200
#define PIN_LEFT_GO 9
#define PIN_LEFT_BACK 8
#define PIN_RIGHT_GO 10
#define PIN_RIGHT_BACK 11
#define PIN_LEFT_SENSOR 6
#define PIN_RIGHT_SENSOR 5
#define LEFT 1
#define RIGHT 2
#define FORE 1
#define BACK 2
#define TRUE 1
#define FALSE 0

unsigned long startTime, endTime;
int shouldStop;

void setPinOutput() {
  pinMode(PIN_LEFT_GO, OUTPUT);
  pinMode(PIN_LEFT_BACK, OUTPUT);
  pinMode(PIN_RIGHT_GO, OUTPUT);
  pinMode(PIN_RIGHT_BACK, OUTPUT);
}

void checkStop() {
  if (millis() < endTime) {
    shouldStop = FALSE;
  }
}

void go(int rol, int dir) {
  int pinGo, pinBack, staGo, staBack, pwm, pwmGo, pwmBack;
  if (shouldStop == TRUE) {
    return;
  }
  if (dir == FORE) {
    if (rol == LEFT) {
      pwm = 70;
      pinGo = PIN_LEFT_GO;
      pinBack = PIN_LEFT_BACK;
    } else if (rol == RIGHT) {
      pwm = 80;
      pinGo = PIN_RIGHT_GO;
      pinBack = PIN_RIGHT_BACK;
    }
    staGo = HIGH;
    pwmGo = pwm;
    staBack = LOW;
    pwmBack = 0;
  } else if (dir == BACK) {
    if (rol == LEFT) {
      pwm = 130;
      pinGo = PIN_LEFT_GO;
      pinBack = PIN_LEFT_BACK;
    } else if (rol == RIGHT) {
      pwm = 255;
      pinGo = PIN_RIGHT_GO;
      pinBack = PIN_RIGHT_BACK;
    }
    staGo = LOW;
    pwmGo = 0;
    staBack = HIGH;
    pwmBack = pwm;
  }
  digitalWrite(pinGo, staGo);
  digitalWrite(pinBack, staBack);
  analogWrite(pinGo, pwmGo);
  analogWrite(pinBack, pwmBack);
}

void stop(int rol) {
  int pinGo, pinBack;
  if (rol == LEFT) {
    pinGo = PIN_LEFT_GO;
    pinBack = PIN_LEFT_BACK;
  } else if (rol == RIGHT) {
    pinGo = PIN_RIGHT_GO;
    pinBack = PIN_RIGHT_BACK;
  }
  Serial.print("Setting ");
  Serial.print(pinGo);
  Serial.print(" to low\n");
  digitalWrite(pinGo, LOW);
  digitalWrite(pinBack, LOW);
  Serial.print("Setting ");
  Serial.print(pinBack);
  Serial.print(" to low\n");
}

int isOn(int rol) {
  int pin, state;
  if (rol == LEFT) {
    pin = PIN_LEFT_SENSOR;
  } else if (rol == RIGHT) {
    pin = PIN_RIGHT_SENSOR;
  }
  state = digitalRead(pin);
  if (state == LOW) {
    // Detected Something
    return TRUE;
  } else if (state == HIGH) {
    // Nothing Found
    return FALSE;
  }
}

void correct() {
  int left, right, triedBack, triedRight, triedLeft;
  triedBack = FALSE;
  triedRight = FALSE;
  do {
    checkStop();
    if (shouldStop == TRUE) {
      return;
    }
    left = isOn(LEFT);
    right = isOn(RIGHT);
    Serial.print("leftDetected:");
    Serial.println(left == TRUE);
    Serial.print("rightDetected:");
    Serial.println(right == TRUE);
    if (left == FALSE && right == FALSE) {
      Serial.println("All clear. Going on...");
      go(RIGHT, FORE);
      go(LEFT, FORE);
    } else {
      Serial.println("Stopping...");
      stop(LEFT);
      stop(RIGHT);
      Serial.println("Going back...");
      go(LEFT, BACK);
      go(RIGHT, BACK);
      delay(BACK_MILLIS);
      Serial.println("Stopping...");
      stop(LEFT);
      stop(RIGHT);
      if (left == FALSE && right == TRUE) {
        Serial.println("Going left...");
        stop(LEFT);
        go(RIGHT, FORE);
        delay(GO_MILLIS);
      } else if (left == TRUE && right == FALSE) {
        Serial.println("Going right...");
        stop(RIGHT);
        go(LEFT, FORE);
        delay(GO_MILLIS);
      } else if (left == TRUE && right == TRUE) {
        if (triedRight == FALSE) {
          Serial.println("Trying right...");
          stop(RIGHT);
          go(LEFT, FORE);
          delay(GO_MILLIS);
          triedRight = TRUE;
        } else if (triedLeft == FALSE) {
          Serial.println("Trying left...");
          stop(LEFT);
          go(RIGHT, FORE);
          delay(GO_MILLIS);
          triedLeft = TRUE;
        }
      }
    }
    delay(DETECT_PERIOD);
  } while (shouldStop == FALSE && (left == TRUE || right == TRUE));
}

void setup() {
  Serial.begin(9600);
  setPinOutput();
  startTime = millis();
  endTime = startTime + 1000 * MAX_SECONDS;
  shouldStop = FALSE;
  Serial.print("startTime:");
  Serial.println(startTime);
  Serial.print("endTime:");
  Serial.println(endTime);
}

void loop() {
  while (shouldStop == FALSE) {
    go(LEFT, FORE);
    go(RIGHT, FORE);
    correct();
    checkStop();
  }
  stop(LEFT);
  stop(RIGHT);
  Serial.println("stopped...");
  // back(10);       //后退1s
  // brake(5);       //停止0.5s
  // run(10);        //前进1s
  // brake(5);       //停止0.5s
  // left(10);       //向左转1s
  // right(10);      //向右转1s
  // spin_right(20); //向右旋转2s
  // spin_left(20);  //向左旋转2s
  // brake(5);       //停车
}
