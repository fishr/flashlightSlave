int ledPin = 13;

int battery = 255;

long frame = 0;

unsigned char dipTable1[32];
int dipFrame1 = -1;
unsigned char dipTable2[256];
int dipFrame2 = -1;


void setup()
{
  
  pinMode(ledPin, OUTPUT);
  
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
}

void loop()
{
  if (dipFrame1 < 0 && random(64) < 1)
  {
    dipFrame1 = 0;
  }
  if (dipFrame2 < 0 && random(256) < 1)
  {
    dipFrame1 = 0;
  }

  int brightness = (battery+255)/2;
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

  analogWrite(ledPin, brightness);

  ++frame;
  if (frame % 120 == 0 && battery > 0)
  {
    battery -= 1;
  }
  
  delay(15);
}

