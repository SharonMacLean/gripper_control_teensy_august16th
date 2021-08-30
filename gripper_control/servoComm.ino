#define HWSERIAL_MOTOR Serial5

XYZrobotServo servo(HWSERIAL_MOTOR, 1);
XYZrobotServoStatus servoStatus;

#define switchPin A12    //gripper limit switch pin

int servoSpeed = 0;
int homePosInRev = 0;
bool revComplete = true;
int enteredFrom;
int lastPos = 0;
float servoPosition;
float tGainP;
float tGainI;
float tScale;
float pGain;
float revs;
float maxPWM;

const int bufSize = 10;
const float maxdt = 1.00;

float tBuff[10];
float lastAction = 0;
float lastError = 0;
float lastT = 0;

void servoSetup(){
  HWSERIAL_MOTOR.begin(57600);
  tGainP = configParams.tGainP;
  tGainI = configParams.tGainI;
  tScale = configParams.tScale;
  pGain = configParams.pGain;
  pinMode(switchPin, INPUT);
  homeServo();
  //servo.writeMaxPwmRam(1);
}
//float torqueControl(float torque){
  
//}

float torqueControl(float torqueGoal){
  servoStatus = servo.readStatus();
  float torque = servoStatus.iBus/200;
  uint8_t speedBuf[2];
  servo.ramRead(74,speedBuf,2);
  int speedMotor = byteToInt(speedBuf);
  if(speedMotor < 0){
    torque *= -1;
  }
  float smoothTorque = torqueBuffer(torque);
  float motorSpeed = (torqueGoal-smoothTorque)*tGainP;
  //float motorSpeed = torquePI(smoothTorque, torqueGoal);
  servo.setSpeed(motorSpeed);
}

float torqueBuffer(float torque){
  float temp = 0;
  for(int i = bufSize-1; i > 0; i--){
    tBuff[i] = tBuff[i-1];
    temp += tBuff[i];
  }
  tBuff[0] = torque;
  temp += tBuff[0];
  return temp/bufSize;
}

float torquePI(float torque, float torqueGoal){
  
  long t = micros();
  float dt = (t-lastT)/1000000;
  if(dt > maxdt){
    dt = maxdt;
  }
  float error = torque-torqueGoal;
  float action = lastAction + tGainP*(error - lastError) + tGainI*error*(dt);
  lastAction = action;
  lastError = error;
  lastT = t;
  return action;
}

/*
float torqueControl(float torqueGoal){
  servoStatus = servo.readStatus();
  float torque = servoStatus.iBus;
  servoSpeed += (torqueGoal/tScale-torque)*tGain;
  servo.setSpeed(servoSpeed);
  return torque*tScale;
}
*/

void speedControl(float speedGoal){
  servoStatus = servo.readStatus();
  float rotSpeed = (speedGoal/15)/(70/60)*1024;
  servo.setSpeed((int)rotSpeed);
  trackPosition();
}

float posControl(float posGoal){
  servoStatus = servo.readStatus();
  int action = (posGoal-servoPosition)*pGain;
  if(action > 1023){ action = 1023; };
  if(action < -1023){ action = -1023; };
  servo.setSpeed(action);
  return trackPosition();
}

float trackPosition(){
  int posInRev = servoStatus.position;
  //int posInRev = (posInput + lastPos)/2;
  if ((posInRev > 1000 && revComplete == true) || (posInRev < 10 && revComplete == true)){
    if(enteredFrom > 500){
      revs += 0.5;
    }
    else{
      revs -= 0.5;
    }
    revComplete = false;
    posInRev = 1020*.55;
  }
  else if(posInRev > 10 && posInRev < 1000 && revComplete == false){
    if (posInRev > 500){
      revs -= 0.5;
    }
    else{
      revs += 0.5;
    }
    revComplete = true;
    enteredFrom = posInRev;
  }
  else if((posInRev > 1000 && revComplete == false)|| (posInRev < 10 && revComplete == false)){
    posInRev = 1020*.55;
  }
  else if(posInRev > 10 && posInRev < 1000){
    enteredFrom = posInRev;
  }
  servoPosition = (posInRev-homePosInRev)*(15/(1020*1.1))+15*revs;
  return servoPosition;
}

void homeServo(){
  speedControl(-10);
  while(digitalRead(switchPin) == LOW){
    delay(1);
  }
  speedControl(10);
  delay(1400);
  speedControl(-2);
  while(digitalRead(switchPin) == LOW){
    delay(1);
  }
  speedControl(0);
  servoStatus = servo.readStatus();
  servoPosition = 0;
  revs = 0;
  homePosInRev = servoStatus.position;
  if(homePosInRev > 1024){ homePosInRev = 0; };
  speedControl(0);
}

void noTorque(){
  servo.torqueOff();
}

XYZrobotServoStatus getServoStatus(){
  return servo.readStatus();
}

void torqueSwitch(bool mode){
  if(mode == false){
    servo.setSpeed(0);
    servo.writeMaxPwmRam(1023);
  }
  else{
    servo.writeMaxPwmRam(0);
    servo.setSpeed(1023);
  }
}

void rebootServo(){
  servo.reboot();
  servoSpeed = 0;
  lastPos = 0;
  lastAction = 0;
  lastError = 0;
  lastT = 0;
}

byte getError(){
  servoStatus = servo.readStatus();
  return servoStatus.statusError;
}
