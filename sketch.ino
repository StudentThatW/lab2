#include <LedControl.h>

struct cell {
   int x;
   int y;
};

struct shift {
   int x_shift;
   int y_shift;
};

struct shift CreateShift(int a, int b) {
   struct shift new_shift;
   new_shift.x_shift = a;
   new_shift.y_shift = b;
   return new_shift;
}

struct cell CreateCell(int a, int b) {
   struct cell new_cell;
   new_cell.x = a;
   new_cell.y = b;
   return new_cell;
}

String commandarray[4] = {"Up", "Down", "Right", "Left"};
shift shiftarray[4] = {CreateShift(0, 1), CreateShift(0, -1), CreateShift(1, 0), CreateShift(-1, 0)};

const int displaysCount = 4;
const int dataPin = 12;
const int clkPin = 10;
const int csPin = 11;
LedControl lc = LedControl(dataPin, clkPin, csPin, displaysCount);

const byte rowAmount = 4;
const byte colAmount = 4;

bool firstMatrix[16][16];
bool secondMatrix[16][16];

String keyMatrix[rowAmount][colAmount];

static bool keyDownMatrix[rowAmount][colAmount];

byte rowPins[rowAmount] = { 5, 4, 3, 2 };
byte colPins[colAmount] = { 6, 7, 8, 9 };


void setup()
{
  for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
        keyMatrix[i][j] = "None";
  keyMatrix[0][1] = "Up";
  keyMatrix[1][0] = "Left";
  keyMatrix[1][2] = "Right";
  keyMatrix[2][1] = "Down";
  keyMatrix[2][1] = "Fixation";
  keyMatrix[3][3] = "Go";
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

cell curent = CreateCell(0, 0);

void loop()
{
  int currentDisplay = 0;
  int x = curent.x;
  int y = curent.y;
  GetExactLocation(currentDisplay, x, y);  
  lc.setLed(currentDisplay, x, y, true);
  String key = getKey();
  if(key == "Fixation")
  {
     firstMatrix[curent.x][ curent.y] = true;     
     return;
  }
  if(key == "Go")
  {
    while(true)
    {
      PlayGame();      
      for(int i = 0; i < 16; i++)
      {
        for(int j = 0; j < 16; j++)
        {
            int z = i;
            int w = j;
            GetExactLocation(currentDisplay, z, w);              
            lc.setLed(currentDisplay, z, w,  firstMatrix[i][j]);
        }
      }
      delay(500);
    }
  }
  if (key != "None") {
    if (!firstMatrix[curent.x] [curent.y])
      lc.setLed(currentDisplay, x, y, false);    
    int place = 0;
    for(int j = 0; j < 4; j++)
    {
      if (commandarray[j] == key)
         place = j;
    }
    int newXShift = shiftarray[place].x_shift;
    int newYShift = shiftarray[place].y_shift;
    int newX = (curent.x + newXShift) % 16;
    int newY = (curent.y + newYShift) % 16;
    if (newX < 0)    
        newX += 16;  
    if (newY < 0)
        newY += 16;
    curent = CreateCell(newX, newY);
  }
}


void GetExactLocation (int & currentDisplay, int & a, int & b)
{
    if (a <= 7)
    {
      if (b <= 7)
         currentDisplay = 0;
      else
         currentDisplay = 1;      
    }
    else
    {
      if (b <= 7)
         currentDisplay = 3;
      else
         currentDisplay = 2;      
    }
    
    if (currentDisplay == 1)
        b = b % 8;
    if (currentDisplay == 2)
    {
        a = 15 - a;
        b = 15 - b;
    }
  
    if (currentDisplay == 3)
    {
        a = 15 - a;
        b = 7 - b;
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
String getKey()
{
  String result = "None";
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
