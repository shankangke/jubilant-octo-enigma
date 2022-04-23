#define PIN_KEY A2
#define PIN_BEEP A3
#define PIN_LEFT_GO 9 // pwm
#define PIN_LEFT_BACK 8
#define PIN_RIGHT_GO 10   // pwm
#define PIN_RIGHT_BACK 11 // pwm
#define PIN_LEFT_SENSOR 6
#define PIN_RIGHT_SENSOR 5

typedef struct {
  int leftGoSta;
  int leftGoPWM;
  int leftBackSta;
  int leftBackPWM;
  int rightGoSta;
  int rightGoPWM;
  int rightBackSta;
  int rightBackPWM;
} action;

void applyAction(action act) {
  digitalWrite(PIN_LEFT_GO, act.leftGoSta);
  analogWrite(PIN_LEFT_GO, act.leftGoPWM);
  digitalWrite(PIN_LEFT_BACK, act.leftBackSta);
  analogWrite(PIN_LEFT_BACK, act.leftBackPWM);
  digitalWrite(PIN_RIGHT_GO, act.rightGoSta);
  analogWrite(PIN_RIGHT_GO, act.rightGoPWM);
  digitalWrite(PIN_RIGHT_BACK, act.rightBackSta);
  analogWrite(PIN_RIGHT_BACK, act.rightBackPWM);
}

action parseAction(String msg) {
  action result = {0, 0, 0, 0, 0, 0, 0, 0};
  int begin = 0, end = 0, index = 0;
  int args[8] = {0};
  while ((begin = end + 1) && (end = msg.indexOf(',', begin)) && (end != -1)) {
    args[index] = msg.substring(begin, end).toInt();
    ++index;
  }
  result.leftGoSta = args[0];
  result.leftGoPWM = args[1];
  result.leftBackSta = args[2];
  result.leftBackPWM = args[3];
  result.rightGoSta = args[4];
  result.rightGoPWM = args[5];
  result.rightBackSta = args[6];
  result.rightBackPWM = args[7];
  return result;
}

String generateStatus() {
  String msg = "";
  msg += (digitalRead(PIN_LEFT_SENSOR) == LOW) ? "1," : "0,";
  msg += (digitalRead(PIN_RIGHT_SENSOR) == LOW) ? "1," : "0,";
  msg += ".";
  return msg;
}

void setPinOutput() {
  pinMode(PIN_BEEP, OUTPUT);
  pinMode(PIN_LEFT_GO, OUTPUT);
  pinMode(PIN_LEFT_BACK, OUTPUT);
  pinMode(PIN_RIGHT_GO, OUTPUT);
  pinMode(PIN_RIGHT_BACK, OUTPUT);
}

void setup() {
  setPinOutput();
  Serial.begin(9600);
}

void loop() {
  static action act = {0, 0, 0, 0, 0, 0, 0, 0};
  static bool unmanned = false;
  static unsigned long int last = 0;
  if (Serial.available()) {
    String received = Serial.readStringUntil('.');
    if(received == "comm-check"){
      Serial.println("comm-check-ok.");
    }else if (received == "start") {
    }else if(received == "stop"){
      act = {0,0,0,0,0,0,0,0};
    }else if(received == "status"){
      Serial.println(generateStatus());
    } else if(received.endsWith(",")) {
      act = parseAction(received);
    }
    last = millis();
  }else{
    if(millis()-last>300){
      act = {0,0,0,0,0,0,0,0};
    }
  }
  applyAction(act);
}
