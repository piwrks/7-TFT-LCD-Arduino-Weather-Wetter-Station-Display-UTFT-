char uCase(char c)
{
  if ((c>='a') && (c<='z'))
    return (c-0x20);
  else
    return c;
}

void buttonWait(int x, int y)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
  waitForTouchRelease();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
}

byte calcDOW(byte d, byte m, int y)
{
  int dow;
  byte mArr[12] = {6,2,2,5,0,3,5,1,4,6,2,4};
  
  dow = (y % 100);
  dow = dow*1.25;
  dow += d;
  dow += mArr[m-1];
  if (((y % 4)==0) && (m<3))
    dow -= 1;
  while (dow>7)
    dow -= 7;
    
  return dow;
}

void waitForTouchRelease()
{
  // Wait for release
  while (myTouch.dataAvailable()==true)
    myTouch.read();
}


