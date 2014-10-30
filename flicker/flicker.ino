int ledPin = 13;

// Variables relevant to the battery
int battery = 255;
volatile boolean usingBattery = true;

// These are lookup tables for flickers
byte dipTable1[32];
int dipFrame1 = -1;
byte dipTable2[256];
int dipFrame2 = -1;

// These are used to keep timing
long frame = 0;
unsigned long lastMilli;

// These are lookup tables for blackout
volatile int blackoutFrame = 256;
byte boTable[256];

void blackout()
{
  blackoutFrame = -random(20);
}

void setup()
{
  // TODO: Take this out
  Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);

  // Set up the lookup tables for the flickers  
  for (int i = 0; i < 32; ++i)
  {
    if (i < 16)
      dipTable1[i] = 255-i*15;
    else
      dipTable1[i] = 255-(32-i)*15;
  }
  for (int i = 0; i < 256; ++i)
  {
    if (i < 32)
      dipTable2[i] = 255-i*4;
    else if (i < 224)
      dipTable2[i] = 255-32*4;
    else
      dipTable2[i] = 255-(256-i)*4;
  }
  // Blackout flicker
  for (int i = 0; i < 256; ++i)
  {
    if (i < 8)
      boTable[i] = 255-i*16;
    else if (i < 24)
      boTable[i] = 255-(24-i)*4-64;
    else if (i < 32)
      boTable[i] = 255-64-(i-24)*23;
    else if (i < 256-24)
      boTable[i] = 0;
    else
      boTable[i] = 255-(256-i)*10;
  }
  
  lastMilli = millis();
}

void loop()
{
  if (millis() - lastMilli >= 15)
  {
    // This stuff does the flickering
    if (dipFrame1 < 0 && random(64) < 1)
      dipFrame1 = 0;
    if (dipFrame2 < 0 && random(256) < 1)
      dipFrame2 = 0;
  
    int brightness = 0;
    if (battery > 45)
      brightness = (battery + 255) / 2;
    else
      brightness = battery*3;
    
    if (dipFrame1 >= 0)
    {
      brightness *= dipTable1[dipFrame1];
      brightness /= 255;
      dipFrame1 += 1;
  
      if (dipFrame1 >= 32)
      {
        dipFrame1 = -1;
      }
    }
    if (dipFrame2 >= 0)
    {
      brightness *= dipTable2[dipFrame2];
      brightness /= 255;
      dipFrame2 += 1;
  
      if (dipFrame2 >= 256)
      {
        dipFrame2 = -1;
      }
    }
    
    if (blackoutFrame < 256)
    {
      if (blackoutFrame >= 0)
      {
        brightness *= boTable[blackoutFrame];
        brightness /= 255;
      }
      blackoutFrame += random(2);
    }
  
    if (usingBattery)
    {
      analogWrite(ledPin, brightness);
      if (frame % 120 == 0 && battery > 0)
      {
        battery -= 1;
        if (battery % 50 == 0)
        {
          // TODO: Transmit the battery level to master
        }
      }
    }
    else
    {
      analogWrite(ledPin, 0);
    }
  
    ++frame;
    lastMilli += 15;
  }
}


// TODO: Replace with actual receive byte
void serialEvent()
{
  while (Serial.available())
  {
    char inChar = (char)Serial.read();
    if (inChar == 'b')
      blackout();
  }
}
