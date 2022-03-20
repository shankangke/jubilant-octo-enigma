#define TIME_DETECT_MILLIS 1
#define PIN_LEFT_GO 9
#define PIN_LEFT_BACK 8
#define PIN_RIGHT_GO 10
#define PIN_RIGHT_BACK 11
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
  { LOW, 0, HIGH, 255, HIGH, 255, LOW, 0, 5 }
#define ACT_SPIN_RIGHT                                                         \
  { HIGH, 255, LOW, 0, LOW, 0, HIGH, 255, 5 }
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

action actGo() {
  action act = ACT_GO;
  return act;
}
action actStop() {
  action act = ACT_STOP;
  return act;
}
action actBack() {
  action act = ACT_BACK;
  return act;
}
action actSpinRight() {
  action act = ACT_SPIN_RIGHT;
  return act;
}
action actSpinLeft() {
  action act = ACT_SPIN_LEFT;
  return act;
}
action decideAction(status state, action currentAction) {
  if (state.leftDetected || state.rightDetected) {
    if (state.leftDetected && !state.rightDetected) {
      return actSpinLeft();
    } else if (!state.leftDetected && state.rightDetected) {
      return actSpinRight();
    } else {
      if (currentAction.leftGoSta == HIGH || currentAction.rightGoSta == HIGH) {
        return actStop();
      } else {
        return actBack();
      }
    }
  } else {
    return actGo();
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
void setup() { setPinOutput(); }

void loop() {
  static action currentAct = actStop();
  currentAct = decideAction(detectStatus(), currentAct);
  applyAction(currentAct);
  delay(TIME_DETECT_MILLIS);
}
