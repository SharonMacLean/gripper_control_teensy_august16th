/*
#define forceSensorMode 0
#define flexSensorMode 1
#define sensFlexID1 10
#define sensFlexID2 11
#define sensC1 20
#define sensC1 21
#define tGain 30
*/
struct EEPROMStruct{
  char forceSensorMode;
  char flexSensorMode;
  char sensForceID1;
  char sensForceID2;
  char sensFlexID1;
  char sensFlexID2;
  float sensC1[50];
  float sensC2[50];
  float tGainP;
  float tGainI;
  float tScale;
  float pGain;
};

struct EEPROMStruct configParams;

float C1[50] = {-123.047,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0,
                0,0,0,0,0,0,0,0,0,0};

float C2[50] = {.25006,1,1,1,1,1,1,1,1,1,
                1,1,1,1,1,1,1,1,1,1,
                1,1,1,1,1,1,1,1,1,1,
                1,1,1,1,1,1,1,1,1,1,
                1,1,1,1,1,1,1,1,1,1};

void EEPROMSetup(){
  //EEPROM.get(0,configParams);
  configParams.forceSensorMode = 1;
  configParams.flexSensorMode = 2;
  configParams.sensForceID1 = 0;
  configParams.sensForceID2 = 1;
  configParams.sensFlexID1 = 2;
  configParams.sensFlexID2 = 3;
  for(int i = 0; i < 50; i++){
    configParams.sensC1[i] = C1[i];
    configParams.sensC2[i] = C2[i];
  }
  configParams.tGainP = 100;
  configParams.tGainI = 0.01;
  configParams.tScale = 10;
  configParams.pGain = 100;
}
/*
char readConfig(char param){
  switch(param){
    case forceSensorMode: return configParams.forceSensorMode; break;
    case flexSensorMode: return configParams.flexSensorMode; break;
    case sensFlexID1: return configParams.sensFlexID1; break;
    case sensFlexID2: return configParams.sensFlexID2; break;
    default: return 0; break;
  }
}

float readConfig(char param, char ID){
  switch(param){
    case sensC1: return configParams.sensC1[ID]; break;
    case sensC2: return configParams.sensC2[ID]; break;
    case tGain: return configParams.tGain; break;
    default: return 0; break;
  }
}

void writeConfig(char param, char value){
  
}
*/
void saveConfig(){
  EEPROM.put(0,configParams);
}
