#include <LedControl.h>

struct cell {
   int a;
   int b;
};

struct cell f(int x, int y) {
   struct cell ang;
   ang.a = x;
   ang.b = y;
   return ang;
}

const int displaysCount = 4;
const int dataPin = 12;
const int clkPin = 10;
const int csPin = 11;
LedControl lc = LedControl(dataPin, clkPin, csPin, displaysCount);

cell keyUp = f(0, 1);
cell keyDown = f(0, -1);
cell keyRight = f(1, 0);
cell keyLeft = f(-1, 0);
cell keyNone = f(0, 0);
cell keyFixation = f(100, 100);
cell keyGo = f(-1, -1);

const byte rowAmount = 4;
const byte colAmount = 4;

bool firstMatrix[16][16];
bool secondMatrix[16][16];

cell keyMatrix[rowAmount][colAmount] = {
  {keyNone, keyUp,    keyNone,  keyNone},
  {keyLeft, keyFixation,  keyRight, keyNone},
  {keyNone, keyDown,  keyNone,  keyNone},
  {keyNone, keyNone,  keyNone,  keyGo}
};

static bool keyDownMatrix[rowAmount][colAmount];

byte rowPins[rowAmount] = { 5, 4, 3, 2 };
byte colPins[colAmount] = { 6, 7, 8, 9 };


void setup()
{
  for(int i=0;i<16;i++)
    for(int j=0;j<16;j++)
        firstMatrix[i][j] = false;
  
  for(int i=0;i<16;i++)
    for(int j=0;j<16;j++)
        secondMatrix[i][j] = false;  

  for(int index=0;index<lc.getDeviceCount();index++) {
      lc.shutdown(index, false);
      lc.setIntensity(index, 16);
      lc.clearDisplay(index);
  }

  
  for (int i = 0; i < rowAmount; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
  }
  
  for (int i = 0; i < colAmount; i++) {
    pinMode(colPins[i], INPUT);
    digitalWrite(colPins[i], HIGH);
  }
  Serial.begin(115200);
}

cell curent = f(0, 0);

void loop()
{
  int place = 0;
  
  if (curent.a <= 7 && curent.b <= 7)
      place = 0;
  if (curent.a > 7 && curent.b <= 7)
      place = 3;
  if (curent.a > 7 && curent.b > 7)
      place = 2;
  if (curent.a <= 7 && curent.b > 7)
      place = 1;

  int x = curent.a;
  int y = curent.b;
  if (place == 1)
      y = y % 8;
  if (place == 2)
  {
      x = 15 - x;
      y = 15 - y;
  }

  if (place == 3)
  {
      x = 15 - x;
      y = 7 - y;
  }
    
  lc.setLed(place, x, y, true);
  cell key = getKey();
  if(key.a == keyFixation.a && key.b == keyFixation.b)
  {
     firstMatrix[curent.a][ curent.b] = true;
     Serial.write("Fix");
     Serial.write(curent.a);
     Serial.write(curent.b);
     return;
  }
  if(key.a == keyGo.a && key.b == keyGo.b)
  {
    Serial.write("go");
    while(true)
    {
      PlayGame();
      Serial.write("yes");
      for(int i = 0; i < 16; i++)
      {
        for(int j = 0; j < 16; j++)
        {
            int z = i;
            int w = j;
            
            if (i <= 7 && j <= 7)
                place = 0;
            if (i > 7 && j <= 7)
                place = 3;
            if (i > 7 && j > 7)
                place = 2;
            if (i <= 7 && j > 7)
                place = 1;

            if (place == 1)
                w = w % 8;
            if (place == 2)
            {
                z = 15 - z;
                w = 15 - w;
            }
          
            if (place == 3)
            {
                z = 15 - z;
                w = 7 - w;
            }
              
            lc.setLed(place, z, w,  firstMatrix[i][j]);
        }
      }
      delay(2000);
    }
  }
  if (key.a != keyNone.a || key.b != keyNone.b) {
    if (!firstMatrix[curent.a] [curent.b])
      lc.setLed(place, x, y, false);
    int newX = (curent.a + key.a) % 16;
    int newY = (curent.b + key.b) % 16;
    if (newX < 0)    
        newX += 16;  
    if (newY < 0)
        newY += 16;
    curent = f(newX, newY);
  }
}


void PlayGame(){
  for(int i = 0; i < 16; i++)
  {
    for(int j = 0; j < 16; j++)
    {
      int sum = 0;
        for(int k = -1; k < 2; k++)
          for(int g = -1; g < 2; g++)
            {
              if(k == 0 && g == 0)
                continue;
              int newX = (i + k) % 16;
              int newY = (j+ g) % 16;
              if (newX < 0)              
                  newX += 16;  
              if (newY < 0)
                  newY += 16;  
               if (firstMatrix[newX][newY])
                  sum += 1;
            }
       if (firstMatrix[i][j])
          secondMatrix[i][j] = sum == 2 || sum == 3;
       else
          secondMatrix[i][j] = sum == 3;
    }
  }  
  for(int i = 0; i < 16; i++)
  {
    for(int j = 0; j < 16; j++)
    {
         firstMatrix[i][j]  = secondMatrix[i][j];
    }
  }
}
cell getKey()
{
  cell result = keyNone;
  for (int i = 0; i < rowAmount; i++) {
    for (int j = 0; j < colAmount; j++) {
      bool isDown = isKeyDown(i, j);
      if (!keyDownMatrix[i][j] && isDown) {
        result = keyMatrix[i][j];
      }
      keyDownMatrix[i][j] = isDown;
    } 
  }
  return result;
}

bool isKeyDown(int i, int j)
{
  bool result = false;
  digitalWrite(rowPins[i], LOW);
  if (digitalRead(colPins[j]) == LOW) {
    result = true;
  }
  digitalWrite(rowPins[i], HIGH);
  return result;
}
