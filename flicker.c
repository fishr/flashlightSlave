#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// These functions pretend to be arduino functions
void delay(unsigned long ms)
{
  usleep(ms*1000);
}
void analogWrite(int pin, int value)
{
  printf("%i\n", value);
}

// The real functions go here
int battery = 255;
long frame = 0;
unsigned char dipTable1[64];
int dipFrame1 = -1;
unsigned char dipTable2[256];
int dipFrame2 = -1;

void setup()
{

}

void loop()
{
  // Note that random() is already defined, can't redefine it
  if (dipFrame1 < 0 && rand()%64 < 1)
  {
    dipFrame1 = 0;
  }
  if (dipFrame2 < 0 && rand()%256 < 1)
  {
    dipFrame1 = 0;
  }

  int brightness = (battery+255)/2;
  if (dipFrame1 >= 0)
  {
    brightness *= dipTable1[dipFrame1];
    brightness /= 255;
    dipFrame1 += 1;

    if (dipFrame1 >= 64)
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

  analogWrite(0, brightness);

  ++frame;
  if (frame % 120 == 0 && battery > 0)
  {
    battery -= 1;
  }

  delay(15);
}

int main()
{
  for (int i = 0; i < 64; ++i)
  {
    if (i < 32)
      dipTable1[i] = 255-i*4;
    else
      dipTable1[i] = 255-(64-i)*4;
  }
  for (int i = 0; i < 256; ++i)
  {
    if (i < 32)
      dipTable2[i] = 255-i*2;
    else if (i < 224)
      dipTable2[i] = 255-64;
    else
      dipTable2[i] = 255-(256-i)*2;

  }

  setup();

  while (1)
  {
    loop();
  }
  return 0;
}

