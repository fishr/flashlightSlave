#include <SPI.h>
#define NOP __asm__ __volatile__ ("nop\n\t")

//Slave comm on nano
int CE=8;
int IRQ=9;
int CSN=10;

byte addrRequest = 128;

byte newAddr = 0x06;

byte tester = 0;
//temp start addr is 0xE7E706, need to set tx_addr and rx_addr_0 to this

void setup(){
  delay(1000);
  pinMode(IRQ, INPUT);
  pinMode(CSN, OUTPUT);
  digitalWrite(CSN, HIGH);
  pinMode(CE, OUTPUT);
  digitalWrite(CE,LOW);
  Serial.begin(115200);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  nrfSlavOn();
  delay(5);
  writeReg(0x00,0b00001110);  //powerup
  flushBuffers();
  delay(5);
  digitalWrite(CE, HIGH);
  delay(5);
  Serial.println("that felt good!");
  transmitSpin(addrRequest);
  Serial.println("i did it!");
  setRX(0x06);
  Serial.println("listening");
  spin();
  Serial.println("got it");
  newAddr = verifyData()+1;
  clearStatus();
  setTX(newAddr);
  Serial.print("my new address is ");
  Serial.println(readReg(0x10));
  delay(5);
}

void loop(){
  if(tester==128){
    tester=129;
  }
  transmit(tester);
  spin();
  byte statusReg = readReg(0x07);
  writeReg(0x07, 0b01110000);
  flushBuffers();
  Serial.println(statusReg); 
  delay(2);
  tester++;
}

void nrfSlavOn(){
  writeReg(0x00,0b00001100);  //makes tx mode
  writeReg(0x01,0b00000001); //autoACK for all pipes
  writeReg(0x02,0b00000011); //enable all pipes
  writeReg(0x03,0b00000001);  //3 byte addresses
  writeReg(0x04,0b01001111);// retransmit delay, and up to 15 retries
  writeReg(0x11, 0b00000001);  //payload size max
  writeAddr(0x10, 0x06); //set transmit address
  writeAddr(0x0A, 0x06);  //set rx address for ack
  //writeReg(0x50, 0x73);
  //writeReg(0x1D,0b00000100); //enable dynamic payload and ack payload
  //writeReg(0x1C,0b00000001); //enable dynamic payload for pipe0
  delay(5);
}

void transmitSpin(byte data){
  byte flag=0;
  while(!flag){
    transmit(data);  
    spin();
    if(0b00100000&readReg(0x07)){
      flag=1;
    }else{
      Serial.println("timeout");
    }
    clearStatus();
  }
}

byte getSender(){
  return(readReg(0x07)>>1)&0b00000111;
}

void setTX(byte addr){
  digitalWrite(CE, LOW);
  writeAddr(0x10, addr); //set transmit address
  writeAddr(0x0A, addr);  //set rx address for ack
  writeReg(0x00,0b00001110); //powerup
  flushBuffers();
  digitalWrite(CE, HIGH);
  delayMicroseconds(200);
}

void setRX(byte addr){
  digitalWrite(CE, LOW);
  writeAddr(0x0A, addr); //first receive addr
  writeReg(0x00,0b00001111); //powerup
  flushBuffers();
  digitalWrite(CE, HIGH);
  delayMicroseconds(200);
}

void spin(){
  while(digitalRead(IRQ)==HIGH){}
}

int verifyData(){  //clears status and returns byte if good, else returns -1 (without clearing)
  byte statreg = readReg(0x07);
  if(0b01000000&statreg){
    clearStatus();
    return readrf();
  }else{
    return -1;
  }
}

void clearStatus(){
    writeReg(0x07, 0b01110000);
}

byte writeAddr(byte addr, byte index){
  digitalWrite(CSN, LOW);
  NOP;
  byte incoming = SPI.transfer(writeRegVal(addr)); //first receive addr
  NOP;
  SPI.transfer(index);
  NOP;
  if(addr<0x0C){
    SPI.transfer(0xE7);
    NOP;
    SPI.transfer(0xE7);
    NOP;
  }
  digitalWrite(CSN, HIGH);
  NOP;
  return incoming;
}

byte writeReg(byte addr, byte data){
  digitalWrite(CSN, LOW);
  NOP;
  byte incoming = SPI.transfer(0b00100000|(0b00011111&addr));
  NOP;
  SPI.transfer(data);
  NOP;
  digitalWrite(CSN, HIGH);
  NOP;
  return incoming;
}

byte writeRegVal(byte addr){
  return (0b00100000|(0b00011111&addr));
}

byte readReg(byte addr){
  digitalWrite(CSN, LOW);
  NOP;
  SPI.transfer(0b00011111&addr);
  NOP;
  byte incoming = SPI.transfer(0xFF);
  NOP;
  digitalWrite(CSN, HIGH);
  NOP;
  return incoming;
}

byte transmit(byte data){
  digitalWrite(CSN, LOW);
  NOP;
  SPI.transfer(0b10100000);
  NOP;
  SPI.transfer(data);
  NOP;
  digitalWrite(CSN, HIGH);
}

byte readrf(){
  digitalWrite(CSN, LOW);
  NOP;
  SPI.transfer(0b01100001);
  NOP;
  byte data = SPI.transfer(0xFF);
  NOP;
  digitalWrite(CSN, HIGH);
  NOP;
  writeReg(0x07, 0b01110000);
  return data;
}

void flushBuffers(){
  digitalWrite(CSN, LOW);
  NOP;
  SPI.transfer(0b11100001);
  NOP;
  digitalWrite(CSN, HIGH);
  NOP;
  digitalWrite(CSN, LOW);
  NOP;
  SPI.transfer(0b11100010);
  NOP;
  digitalWrite(CSN, HIGH);
  NOP;
}
