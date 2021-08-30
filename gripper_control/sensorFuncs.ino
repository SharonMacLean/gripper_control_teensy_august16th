#define forcePin1 A3       // load cell 1 pin
#define forcePin2 A0       // load cell 2 pin
#define flexPin1 A2        // flex sensor 1 pin
#define flexPin2 A1        // flex sensor 2 pin


int flexID1;
int flexID2;

float bias[50];
float slope[50];

float resolution = 1023.00;       //10 bit resolution
float maxPinVolt = 3.3;    // V

void sensorSetup(){
  flexID1 = configParams.sensFlexID1;
  flexID2 = configParams.sensFlexID2;
  bias[0] = configParams.sensC1[0];
  bias[1] = configParams.sensC1[1];
  bias[flexID1] = configParams.sensC1[flexID1];
  bias[flexID2] = configParams.sensC1[flexID2];
  slope[0] = configParams.sensC2[0];
  slope[1] = configParams.sensC2[1];
  slope[flexID1] = configParams.sensC2[flexID1];
  slope[flexID2] = configParams.sensC2[flexID2];
  pinMode(forcePin1,INPUT);
  pinMode(forcePin2,INPUT);
  pinMode(flexPin1,INPUT);
  pinMode(flexPin2,INPUT);
}

// Returns the average force from the two load cells 
float getForce(){
  if(configParams.forceSensorMode == 2){
    float f1 = forceTF(analogRead(forcePin1),0);
    float f2 = forceTF(analogRead(forcePin2),1);
    float x = (f1+f2)/2;
    return x;
  }
  else{
    return forceTF(analogRead(forcePin1),0);
  }
}

// Returns the average op amp voltage output from the two load cells (V)
float getForceVoltage(){
  if(configParams.forceSensorMode == 2){
    float f1 = analogRead(forcePin1)/resolution*maxPinVolt;
    float f2 = analogRead(forcePin2)/resolution*maxPinVolt;
    float x = (f1+f2)/2;
    return x;
  }
  else{
    return analogRead(forcePin1)/resolution*maxPinVolt;
  }
}

//int getForceVoltage(){
//  if(configParams.forceSensorMode == 2){
//    int f1 = analogRead(forcePin1);
//    int f2 = analogRead(forcePin2);
//    int x = (f1+f2)/2;
//    return x;
//  }
//  else{
//    return analogRead(forcePin1);
//  }
//}

float getFlex(){
  float f1 = flexTF(analogRead(flexPin1),flexID1);
  float f2 = flexTF(analogRead(flexPin2),flexID2);
  return (f1+f2)/2;
}

// Returns the average voltage output by the two flex sensors (V)
int getFlexVoltage(){
  float f1 = analogRead(flexPin1)/resolution*maxPinVolt;
  float f2 = analogRead(flexPin2)/resolution*maxPinVolt;
  return (f1+f2)/2;
}

//int getFlexVoltage(){
//  int f[2];
//  f[0] = analogRead(flexPin1);
//  f[1] = analogRead(flexPin2);
//  return f[0]+f[1];
//}

float forceTF(int x, char ID){
  return x*slope[ID] + bias[ID];
}

float flexTF(int x, char ID){
  return x*slope[ID] + bias[ID];
}

void jawSwitch(char ID1, char ID2){
  flexID1 = ID1;
  flexID2 = ID2;
  configParams.sensFlexID1 = flexID1;
  configParams.sensFlexID2 = flexID2;
  bias[flexID1] = configParams.sensC1[flexID1];
  bias[flexID2] = configParams.sensC1[flexID2];
  slope[flexID1] = configParams.sensC2[flexID1];
  slope[flexID2] = configParams.sensC2[flexID2];
}
