#define TIME_DETECT_MILLIS 1
#define PIN_KEY A2
#define PIN_BEEP A3
#define PIN_LEFT_GO 9 // pwm
#define PIN_LEFT_BACK 8
#define PIN_RIGHT_GO 10   // pwm
#define PIN_RIGHT_BACK 11 // pwm
#define PIN_LEFT_SENSOR 6
#define PIN_RIGHT_SENSOR 5
#define ACT_SCHEME                                                             \
  {                                                                            \
    LEFT_GO_STA, LEFT_GO_PWM, LEFT_BACK_STA, LEFT_BACK_PWM, RIGHT_GO_STA,      \
        RIGHT_GO_PWM, RIGHT_BACK_STA, RIGHT_BACK_PWM, DURATION                 \
  }
#define ACT_GO                                                                 \
  { HIGH, 70, LOW, 0, HIGH, 75, LOW, 0, 5 }
#define ACT_BACK                                                               \
  { LOW, 0, HIGH, 255, LOW, 0, HIGH, 255, 100 }
#define ACT_SPIN_LEFT                                                          \
  { LOW, 0, HIGH, 0, HIGH, 75, LOW, 0, 5 }
#define ACT_SPIN_RIGHT                                                         \
  { HIGH, 70, LOW, 0, LOW, 0, HIGH, 0, 5 }
#define ACT_STOP                                                               \
  { LOW, 0, LOW, 0, LOW, 0, LOW, 0, 200 }
#define LEFT 1
#define RIGHT 2
#define TRUE 1
#define FALSE 0

typedef struct {
  int leftGoSta;
  int leftGoPWM;
  int leftBackSta;
  int leftBackPWM;
  int rightGoSta;
  int rightGoPWM;
  int rightBackSta;
  int rightBackPWM;
  int duration;
} action;

typedef struct {
  bool leftDetected;
  bool rightDetected;
} status;

void setPinOutput() {
  pinMode(PIN_BEEP, OUTPUT);
  pinMode(PIN_LEFT_GO, OUTPUT);
  pinMode(PIN_LEFT_BACK, OUTPUT);
  pinMode(PIN_RIGHT_GO, OUTPUT);
  pinMode(PIN_RIGHT_BACK, OUTPUT);
}

status detectStatus() {
  status state;
  state.leftDetected = (digitalRead(PIN_LEFT_SENSOR) == LOW) ? true : false;
  state.rightDetected = (digitalRead(PIN_RIGHT_SENSOR) == LOW) ? true : false;
  return state;
}

action actGo(int duration = 0) {
  action act = ACT_GO;
  if (duration != 0) {
    act.duration = duration;
  }
  return act;
}
action actStop(int duration = 0) {
  action act = ACT_STOP;
  if (duration != 0) {
    act.duration = duration;
  }
  return act;
}
action actBack(int duration = 0) {
  action act = ACT_BACK;
  if (duration != 0) {
    act.duration = duration;
  }
  return act;
}
action actSpinRight(int duration = 0) {
  action act = ACT_SPIN_RIGHT;
  if (duration != 0) {
    act.duration = duration;
  }
  return act;
}
action actSpinLeft(int duration = 0) {
  action act = ACT_SPIN_LEFT;
  if (duration != 0) {
    act.duration = duration;
  }
  return act;
}
action decideAction(status state, action currentAction, int duration = 0,
                    bool *unmanned = nullptr, String command = "") {
  if (command != "") {
    if (command == "beep") {
      digitalWrite(PIN_BEEP, HIGH);
      delay(duration);
      digitalWrite(PIN_BEEP, LOW);
    } else if (command == "goon") {
      return actGo(duration);
    } else if (command == "back") {
      return actBack(duration);
    } else if (command == "stop") {
      *unmanned = false;
      return actStop();
    } else if (command == "rspi") {
      return actSpinRight(duration);
    } else if (command == "lspi") {
      return actSpinLeft(duration);
    } else if (command == "auto") {
      *unmanned = true;
    } else {
      return actStop();
    }
  }

  if (unmanned != nullptr && *unmanned) {
    if (state.leftDetected || state.rightDetected) {
      if (state.leftDetected && !state.rightDetected) {
        return actSpinLeft();
      } else if (!state.leftDetected && state.rightDetected) {
        return actSpinRight();
      } else {
        if (currentAction.leftGoSta == HIGH ||
            currentAction.rightGoSta == HIGH) {
          return actStop();
        } else {
          return actBack();
        }
      }
    } else {
      return actGo();
    }
  } else {
    return actStop();
  }
}
void applyAction(action act) {
  digitalWrite(PIN_LEFT_GO, act.leftGoSta);
  analogWrite(PIN_LEFT_GO, act.leftGoPWM);
  digitalWrite(PIN_LEFT_BACK, act.leftBackSta);
  analogWrite(PIN_LEFT_BACK, act.leftBackPWM);
  digitalWrite(PIN_RIGHT_GO, act.rightGoSta);
  analogWrite(PIN_RIGHT_GO, act.rightGoPWM);
  digitalWrite(PIN_RIGHT_BACK, act.rightBackSta);
  analogWrite(PIN_RIGHT_BACK, act.rightBackPWM);
  delay(act.duration);
}

bool enabled(bool currentEnabled) {
  if (digitalRead(PIN_KEY) == HIGH) {
    digitalWrite(PIN_BEEP, HIGH);
    while (digitalRead(PIN_KEY) == HIGH) {
      ;
    }
    digitalWrite(PIN_BEEP, LOW);
    return !currentEnabled;
  } else {
    return currentEnabled;
  }
}

String parseMessage(String message, int *duration = nullptr) {
  *duration = message.substring(4).toInt();
  return message.substring(0, 4);
}

void setup() {
  setPinOutput();
  Serial.begin(9600);
}

void loop() {
  static bool currentEnabled = false; // unmanned
  static action currentAct = actStop();
  currentEnabled = enabled(currentEnabled);
  if (Serial.available()) {
    int duration = 1;
    String command = parseMessage(Serial.readStringUntil('.'), &duration);
    currentAct = decideAction(detectStatus(), currentAct, duration,
                              &currentEnabled, command);
  } else if (currentEnabled) {
    Serial.print("branch 2");
    delay(TIME_DETECT_MILLIS);
    currentAct = decideAction(detectStatus(), currentAct, 0, &currentEnabled);
  } else {
    currentAct = actStop();
  }
  Serial.print(currentEnabled);
  applyAction(currentAct);
}
