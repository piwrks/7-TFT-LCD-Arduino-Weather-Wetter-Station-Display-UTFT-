void drawUpButton(int x, int y)
{
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(128, 128, 255);
  for (int i=0; i<15; i++)
    myGLCD.drawLine(x+6+(i/1.5), y+20-i, x+27-(i/1.5), y+20-i);
}

void drawDownButton(int x, int y)
{
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(x, y, x+32, y+25);
  myGLCD.setColor(128, 128, 255);
  for (int i=0; i<15; i++)
    myGLCD.drawLine(x+6+(i/1.5), y+5+i, x+27-(i/1.5), y+5+i);
}

void showDOW(byte dow)
{
  char* str[] = {"MON","TUE","WED","THU","FRI","SAT","SUN"};
  
  myGLCD.setColor(128, 128, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print(str[dow-1], 26, 167);
}

byte validateDate(byte d, byte m, word y)
{
  byte mArr[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
  byte od;
  
  if (m==2)
  {
    if ((y % 4)==0)
    {
      if (d==30)
        od=1;
      else if (d==0)
        od=29;
      else
        od=d;
    }
    else
    {
      if (d==29)
        od=1;
      else if (d==0)
        od=28;
      else
        od=d;
    }
  }
  else
  {
    if (d==0)
      od=mArr[m-1];
    else if (d==(mArr[m-1]+1))
      od=1;
    else
      od=d;
  }
  
  return od;
}

byte validateDateForMonth(byte d, byte m, word y)
{
  byte mArr[12] = {31,0,31,30,31,30,31,31,30,31,30,31};
  byte od;
  boolean dc=false;
  
  if (m==2)
  {
    if ((y % 4)==0)
    {
      if (d>29)
      {
        d=29;
        dc=true;
      }
    }
    else
    {  
      if (d>28)
      {
        d=28;
        dc=true;
      }
    }
  }
  else
  {
    if (d>mArr[m-1])
    {
      d=mArr[m-1];
      dc=true;
    }
  }
  
  if (dc)
  {
    if (d<10)
    {
      myGLCD.printNumI(0, 122, 140);
      myGLCD.printNumI(d, 138, 140);
    }
    else
    {
      myGLCD.printNumI(d, 122, 140);
    }
  }
  
  return d;
}

void setClock()
{
  Time t_temp;
  int x, y;
  int res = 0;
  boolean ct=false;
  boolean cd=false;
  
  waitForTouchRelease();    
  
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  
  // Draw Save button
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(165, 200, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(165, 200, 319, 239);
  myGLCD.setBackColor(64, 64, 128);
  myGLCD.print("Save", 210, 212);
  myGLCD.setBackColor(0, 0, 0);
  // Draw Cancel button
  myGLCD.setColor(64, 64, 128);
  myGLCD.fillRoundRect(0, 200, 154, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect(0, 200, 154, 239);
  myGLCD.setBackColor(64, 64, 128);
  myGLCD.print("Cancel", 29, 212);
  myGLCD.setBackColor(0, 0, 0);
  
  // Draw frames
  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRoundRect(0, 0, 319, 96);
  myGLCD.drawRoundRect(0, 100, 319, 196);
  myGLCD.print("Time:", 10, 40);
  myGLCD.print(":", 154, 40);
  myGLCD.print(":", 202, 40);
  myGLCD.print("Date:", 10, 140);
  myGLCD.print(".", 154, 140);
  myGLCD.print(".", 202, 140);
  
  // Draw Buttons
  drawUpButton(122, 10);
  drawUpButton(170, 10);
  drawUpButton(218, 10);
  drawDownButton(122, 61);
  drawDownButton(170, 61);
  drawDownButton(218, 61);
  drawUpButton(122, 110);
  drawUpButton(170, 110);
  drawUpButton(234, 110);
  drawDownButton(122, 161);
  drawDownButton(170, 161);
  drawDownButton(234, 161);
  
  // Print current time and date
  myGLCD.setColor(255, 255, 255);
  t_temp = rtc2.getTime();
  if (t_temp.date==0)
  {
    t_temp.date=1;
    t_temp.mon=1;
    t_temp.year=2010;
    t_temp.dow=5;
    t_temp.hour=0;
    t_temp.min=0;
    t_temp.sec=0;
    ct=true;
    cd=true;
  }
  
  if (t_temp.hour<10)
  {
    myGLCD.printNumI(0, 122, 40);
    myGLCD.printNumI(t_temp.hour, 138, 40);
  }
  else
  {
    myGLCD.printNumI(t_temp.hour, 122, 40);
  }
  if (t_temp.min<10)
  {
    myGLCD.printNumI(0, 170, 40);
    myGLCD.printNumI(t_temp.min, 186, 40);
  }
  else
  {
    myGLCD.printNumI(t_temp.min, 170, 40);
  }
  if (t_temp.sec<10)
  {
    myGLCD.printNumI(0, 218, 40);
    myGLCD.printNumI(t_temp.sec, 234, 40);
  }
  else
  {
    myGLCD.printNumI(t_temp.sec, 218, 40);
  }
  if (t_temp.date<10)
  {
    myGLCD.printNumI(0, 122, 140);
    myGLCD.printNumI(t_temp.date, 138, 140);
  }
  else
  {
    myGLCD.printNumI(t_temp.date, 122, 140);
  }
  if (t_temp.mon<10)
  {
    myGLCD.printNumI(0, 170, 140);
    myGLCD.printNumI(t_temp.mon, 186, 140);
  }
  else
  {
    myGLCD.printNumI(t_temp.mon, 170, 140);
  }
  myGLCD.printNumI(t_temp.year, 218, 140);
  showDOW(t_temp.dow);
  
  while (res==0)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      if ((y>=10) && (y<=35)) // Buttons: Time UP
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 10);
          t_temp.hour+=1;
          if (t_temp.hour==24)
            t_temp.hour=0;
          if (t_temp.hour<10)
          {
            myGLCD.printNumI(0, 122, 40);
            myGLCD.printNumI(t_temp.hour, 138, 40);
          }
          else
          {
            myGLCD.printNumI(t_temp.hour, 122, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 10);
          t_temp.min+=1;
          if (t_temp.min==60)
            t_temp.min=0;
          if (t_temp.min<10)
          {
            myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_temp.min, 186, 40);
          }
          else
          {
            myGLCD.printNumI(t_temp.min, 170, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(218, 10);
          t_temp.sec+=1;
          if (t_temp.sec==60)
            t_temp.sec=0;
          if (t_temp.sec<10)
          {
            myGLCD.printNumI(0, 218, 40);
            myGLCD.printNumI(t_temp.sec, 234, 40);
          }
          else
          {
            myGLCD.printNumI(t_temp.sec, 218, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
      }
      else if ((y>=61) && (y<=86)) // Buttons: Time DOWN
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 61);
          t_temp.hour-=1;
          if (t_temp.hour==255)
            t_temp.hour=23;
          if (t_temp.hour<10)
          {
            myGLCD.printNumI(0, 122, 40);
            myGLCD.printNumI(t_temp.hour, 138, 40);
          }
          else
          {
            myGLCD.printNumI(t_temp.hour, 122, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 61);
          t_temp.min-=1;
          if (t_temp.min==255)
            t_temp.min=59;
          if (t_temp.min<10)
          {
            myGLCD.printNumI(0, 170, 40);
            myGLCD.printNumI(t_temp.min, 186, 40);
          }
          else
          {
            myGLCD.printNumI(t_temp.min, 170, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(218, 61);
          t_temp.sec-=1;
          if (t_temp.sec==255)
            t_temp.sec=59;
          if (t_temp.sec<10)
          {
            myGLCD.printNumI(0, 218, 40);
            myGLCD.printNumI(t_temp.sec, 234, 40);
          }
          else
          {
            myGLCD.printNumI(t_temp.sec, 218, 40);
          }
          if (ct==false)
          {
            ct=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 10);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
      }
      if ((y>=110) && (y<=135)) // Buttons: Date UP
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 110);
          t_temp.date+=1;
          t_temp.date=validateDate(t_temp.date, t_temp.mon, t_temp.year);
          if (t_temp.date<10)
          {
            myGLCD.printNumI(0, 122, 140);
            myGLCD.printNumI(t_temp.date, 138, 140);
          }
          else
          {
            myGLCD.printNumI(t_temp.date, 122, 140);
          }
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 110);
          t_temp.mon+=1;
          if (t_temp.mon==13)
            t_temp.mon=1;
          if (t_temp.mon<10)
          {
            myGLCD.printNumI(0, 170, 140);
            myGLCD.printNumI(t_temp.mon, 186, 140);
          }
          else
          {
            myGLCD.printNumI(t_temp.mon, 170, 140);
          }
          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(234, 110);
          t_temp.year+=1;
          if (t_temp.year==2100)
            t_temp.year=2000;
          myGLCD.printNumI(t_temp.year, 218, 140);
          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        t_temp.dow=calcDOW(t_temp.date, t_temp.mon, t_temp.year);
        showDOW(t_temp.dow);
      }
      else if ((y>=161) && (y<=186)) // Buttons: Date DOWN
      {
        if ((x>=122) && (x<=154))
        {
          buttonWait(122, 161);
          t_temp.date-=1;
          t_temp.date=validateDate(t_temp.date, t_temp.mon, t_temp.year);
          if (t_temp.date<10)
          {
            myGLCD.printNumI(0, 122, 140);
            myGLCD.printNumI(t_temp.date, 138, 140);
          }
          else
          {
            myGLCD.printNumI(t_temp.date, 122, 140);
          }
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=170) && (x<=202))
        {
          buttonWait(170, 161);
          t_temp.mon-=1;
          if (t_temp.mon==0)
            t_temp.mon=12;
          if (t_temp.mon<10)
          {
            myGLCD.printNumI(0, 170, 140);
            myGLCD.printNumI(t_temp.mon, 186, 140);
          }
          else
          {
            myGLCD.printNumI(t_temp.mon, 170, 140);
          }
          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        else if ((x>=218) && (x<=250))
        {
          buttonWait(234, 161);
          t_temp.year-=1;
          if (t_temp.year==1999)
            t_temp.year=2099;
          myGLCD.printNumI(t_temp.year, 218, 140);
          t_temp.date=validateDateForMonth(t_temp.date, t_temp.mon, t_temp.year);
          if (cd==false)
          {
            cd=true;
            myGLCD.setColor(64, 64, 128);
            myGLCD.setFont(SmallFont);
            myGLCD.print("Changed!", 10, 110);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setFont(SmallFont);
          }
        }
        t_temp.dow=calcDOW(t_temp.date, t_temp.mon, t_temp.year);
        showDOW(t_temp.dow);
      }
      else if ((y>=200) && (y<=239)) // Buttons: CANCEL / SAVE
      {
        if ((x>=165) && (x<=319))
        {
          res=1;
          myGLCD.setColor (255, 0, 0);
          myGLCD.drawRoundRect(165, 200, 319, 239);
        }
        else if ((x>=0) && (x<=154))
        {
          res=2;
          myGLCD.setColor (255, 0, 0);
          myGLCD.drawRoundRect(0, 200, 154, 239);
        }
      }
    }
  }
  
  waitForTouchRelease();

  if (res==1)
  {
    if (ct)
      rtc2.setTime(t_temp.hour, t_temp.min, t_temp.sec);
    if (cd)
    {
      rtc2.setDate(t_temp.date, t_temp.mon, t_temp.year);
      rtc2.setDOW(t_temp.dow);
    }
  }
  
myGLCD.clrScr();

 //Beschriftung und Linien hinzufügen
    
 myGLCD.setBackColor(0, 0, 0);
 myGLCD.setColor(255, 255, 255);
 myGLCD.drawLine(0, 15, 799, 15);
 myGLCD.drawLine(410, 15, 410, 480);
 myGLCD.drawLine(0, 290, 410, 290);
 myGLCD.drawLine(200, 15, 200, 480);
 myGLCD.drawLine(410, 215, 799, 215);
 myGLCD.drawLine(600, 335, 600, 480);
 myGLCD.drawLine(410, 335, 600, 335);
 
 myGLCD.setFont(BVS_15);
 
 myGLCD.print("Innen - Klima", 50, 30, 0);
 myGLCD.print("Grad C", 70, 105, 0);
 myGLCD.print("Feuchte %", 50, 185, 0);
   
 myGLCD.print("Aussen - Klima", 250, 30, 0);
 myGLCD.print("Grad C", 270, 105, 0);
 myGLCD.print("Feuchte %", 250, 185, 0);
 
 myGLCD.print("Barometer", 550, 230, 0);
 myGLCD.print("hPa", 580, 305, 0); 
 
 myGLCD.setFont(BVS_9); 
 myGLCD.print("Tendenz hPa 1-Std", 650, 385, 0);   
 myGLCD.print("Tendenz hPa 3-Std", 650, 455, 0);  

 myGLCD.setFont(BVS_15);
 myGLCD.print("Taupunkt", 48, 265, 0);
 myGLCD.print("Aktuell:", 480, 30, 0);
 myGLCD.print("Vorhersage:", 640, 30, 0);
 
 myGLCD.print("Mondphase", 255, 310, 0);
 
 myGLCD.print("Heizung", 475, 350, 0);
 
 myGLCD.setFont(BVS_9); 
 myGLCD.print("Puffer oben", 420, 380, 0);
 myGLCD.print("Puffer mitte", 420, 395, 0); 
 myGLCD.print("Vorlauf", 420, 410, 0);
 myGLCD.print("Ruecklauf", 420, 425, 0); 
 myGLCD.print("Kessel", 420, 440, 0);
 myGLCD.print("Schlafzimmer", 420, 455, 0); 
 myGLCD.print(":", 520, 380, 0);
 myGLCD.print(":", 520, 395, 0); 
 myGLCD.print(":", 520, 410, 0);
 myGLCD.print(":", 520, 425, 0); 
 myGLCD.print(":", 520, 440, 0);
 myGLCD.print(":", 520, 455, 0);
  
 
}


