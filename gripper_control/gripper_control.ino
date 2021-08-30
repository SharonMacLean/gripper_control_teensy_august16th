#include <XYZrobotServo.h>
#include <EEPROM.h>

struct packet{
  byte cmd;
  byte data[256];
  byte dataSize;
};

struct packet packIn;
struct packet packOut;

char loopState = 0;
float posSetP = 0;
float jawPosition = 0;
float torSetP = 0;
float initialForce = 0;
float velSetP = 0;
float motorTorque = 0;
byte dataIn[4];

void setup() {
  // put your setup code here, to run once:
  EEPROMSetup();
  serialSetup();
  servoSetup();
  sensorSetup();
  delay(5000);
}

void loop() {
  //Serial.println("looping");
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    getPacket();
    //Serial.print("recieved");
    switch (packIn.cmd){
      case 0x01: //fullopen
        loopState = 0;
        homeServo();
        break;
      case 0x02: //open
        loopState = 1;
        for(int i = 0; i < 4; i++){
          dataIn[i] = packIn.data[i];
        }
        posSetP = byteToFloat(dataIn);
        //torqueSwitch(false);
        break;
      case 0x03: //hold
        loopState = 1;
        posSetP = trackPosition();
        //torqueSwitch(false);
        break;
      case 0x04: //loose
        loopState = 0;
        noTorque();
        break;
      case 0x05: //GetInfo
        sendParams(packIn.data);
        break;
      case 0x06: //UpdateControl
        loopState = 2;
        for(int i = 0; i < 4; i++){
          dataIn[i] = packIn.data[i];
        }
        torSetP = byteToFloat(dataIn);
        //torqueSwitch(true);
        break;
      case 0x07: //Close
        loopState = 3;
        initialForce = getForce();
        ///torqueSwitch(false);
        break;
      case 0x08: //ResendData
        //unused
        break;
      case 0x09: //ContactMade
        //unused
        break;
      case 0x0A: //SetPos
        loopState = 1;
        for(int i = 0; i < 4; i++){
          dataIn[i] = packIn.data[i];
        }
        posSetP = 75 - byteToFloat(dataIn)/2;
        torqueSwitch(false);
        break;
      case 0x0B: //SetVel
        loopState = 0;
        for(int i = 0; i < 4; i++){
          dataIn[i] = packIn.data[i];
        }
        velSetP = byteToFloat(dataIn);
        speedControl(velSetP);
        torqueSwitch(false);
        break;
      case 0x0C:
        rebootServo();
        break;
      default: //error
        //acton
        break;
    }
  }
  switch (loopState){
    case 0x01: //posCont
      jawPosition = posControl(posSetP);
      //Serial.println(jawPosition);
      /*
      if (abs(posSetP - jawPosition) > 1){
        jawPosition = posControl(posSetP);
      }
      else{
        speedControl(0);
      }
      */
      break;
    case 0x02: //torCont
      motorTorque = torqueControl(torSetP);
      break;
    case 0x03: //Close
      speedControl(5);
      if(getForce() > initialForce*1.2){
        speedControl(0);
        loopState = 0;
        struct packet packOut;
        packOut.cmd = 0x09;
        packOut.dataSize = 0x00;
        sendPacket();
      }
      break;
    default:
    //Serial.println(getForce());
      //acton
      break;
  }
}

void sendParams(byte* params){
  packOut.cmd = 0x05;
  XYZrobotServoStatus motorStat = getServoStatus();
  bool finishedParams = false;
  int k = 0; //data pointer
  for(int i = 0; i < 256; i++){
    switch(params[i]){
      case 0x01:{ //Voltage：The voltage currently applied to servo. 
      
      }
        break;
      case 0x02:{ //current
        byte* tempData;
        tempData = floatToByte((float)(motorStat.iBus/200));
        for(int j = 0; j < 4; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x03:{ //duty cycle
        byte* tempData;
        tempData = floatToByte((float)(motorStat.pwm/1024));
        for(int j = 0; j < 4; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x04:{ //theta target (desired servo output angle)
        byte* tempData;
        tempData = intToByte(motorStat.posRef);
        for(int j = 0; j < 2; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x05:{ //theta actual (actual servo output angle)
        byte* tempData;
        tempData = intToByte(motorStat.position);
        for(int j = 0; j < 2; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x06:{ // omega target (desired servo angular speed) 
        // called "omega goal" by the XYZRobot Servo library, goal speed of speed close-loop control
      }
        break;
      case 0x07:{ //omega actual (actual servo angular speed) 
        // called "omega ref" by the XYZRobot Servo library, ref speed of speed close-loop control
      }
        break;
      case 0x08:{ //motor error
        packOut.data[k] = getError();
        k++;
      }
        break;
      case 0x09:{ //load cell op amp output voltage
        byte* tempData;
        tempData = floatToByte(getForceVoltage());
        for(int j = 0; j < 4; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x0A:{ //force calculated from load cell output (N)
        byte* tempData;
        tempData = floatToByte(getForce());
        for(int j = 0; j < 4; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x0B:{ //jaw position
        byte* tempData;
        //Serial.println(jawPosition);
        tempData = floatToByte(trackPosition());
        for(int j = 0; j < 4; j++){
          //Serial.print(tempData[j]);
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x0C:{ //deflection of gripper fingers
        byte* tempData;
        tempData = floatToByte(getFlex());
        for(int j = 0; j < 4; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      case 0x0D:{ //flex sensor voltage
        byte* tempData;
        tempData = floatToByte(getFlexVoltage());
        for(int j = 0; j < 4; j++){
          packOut.data[k] = tempData[j];
          k++;
        }
      }
        break;
      default:
        finishedParams = true;
        break;
    }
    if(finishedParams == true){
      break;
    }
  }
  packOut.dataSize = k;
  sendPacket();


}
