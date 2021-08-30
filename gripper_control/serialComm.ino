void serialSetup(){
  Serial.begin(115200);
  //Serial.setTimeout(1000);
}

struct packet getPacket(){
  //struct packet packIn;
  
  byte bufferIn[256];
  Serial.readBytesUntil(char(0xFF), bufferIn, 256);
  
  packIn.cmd = Serial.read();
  packIn.dataSize = Serial.read();
  Serial.readBytes((char*)packIn.data, (int)packIn.dataSize);
  //Serial.write(packIn.cmd);
  //Serial.write(packIn.dataSize);
  //for(int i = 0; i < packIn.dataSize; i++){
  //  Serial.write(packIn.data[i]);
  //}
  //for(int i = 0; i < 4; i++){
  //  Serial.write(Serial.read());
  //}
  return packIn;
}

void sendPacket(){
  //packOut.dataSize = (byte)sizeof(packOut.data);
  Serial.write(byte(0xFF));
  Serial.write(packOut.cmd);
  Serial.write(packOut.dataSize);
  for(int i = 0; i < packOut.dataSize; i++){
    Serial.write(packOut.data[i]);
  }
}

byte* floatToByte(float x){
  byte* tempData = (byte*)&x;
  return tempData;
}

float byteToFloat(byte* x){
  float tempData = *((float*)x);
  return tempData;
}

// This method translates a decimal number to a hexadecimal number.
// TODO: This method has a crucial error.
// If the resultant hexadecimal number contains a zero, this method discards the zero.
// This results in the translated value being dramatically wrong.
// Ex. 518base10 converts to 0x0206 (hex). However, the intToByte() method outputs 0x0026. 
// (It discards the zero, giving the incorrect value). 
byte* intToByte(int x){
  byte* tempData = (byte*)&x;
  return tempData;
}

int byteToInt(byte* x){
  float tempData = *((int*)x);
  return tempData;
}
