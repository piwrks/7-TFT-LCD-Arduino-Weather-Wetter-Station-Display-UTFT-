void drawverlauf()
{
    
  int x, y;
  int res = 0;
     
 waitForTouchRelease();    
  
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  
  myGLCD.setColor(255, 255, 255);
  
  myGLCD.drawLine(400, 0, 400, 479);
  myGLCD.drawLine(0, 240, 799, 240);
  
  myGLCD.print("Innen - Klima 24h", 140, 5, 0);
  myGLCD.print("Aussen - Klima 24h", 540, 5, 0);
  
  //Skalen Zeichnen
  
  
   
  
   //Graphen zeichen
  
  drawGraphTempInnen(historyTempInnen, 24);

  

  // Zurück zum Hauptbildschirm
    
  while (res==0)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      
       if ((y>=0) && (y<=480)) // Touchbereich um zurückzukehren
      {
        if ((x>=0) && (x<=800))
        {
          res=2;
          myGLCD.setColor (255, 0, 0);
          myGLCD.drawRoundRect(1, 1, 799, 479);
        }
      }
    }
  }
  
  waitForTouchRelease();
  
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
 myGLCD.print("hPa", 660, 305, 0); 
 
 myGLCD.setFont(BVS_9); 
 myGLCD.print("Tendenz hPa 1-Std", 650, 385, 0);   
 myGLCD.print("Tendenz hPa 3-Std", 650, 455, 0);  

 myGLCD.setFont(BVS_15);
 myGLCD.print("Taupunkt", 48, 265, 0);
 myGLCD.print("Aktuell:", 480, 30, 0);
 myGLCD.print("Vorhersage:", 645, 30, 0);
 
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
  
  renderDatum();
  renderTempInnen();
  renderTempAussen();
  renderBarometer();
  rendertaupunkt();
  renderhumidex();
  renderaktuell();
  renderforecast();
  rendermoon();
  rendermoondays();
  rendersonne();
  renderheizung(); 
  drawMillibarGraph(historyBaro, 24);
 
}

//******************* BARGRAPHEN ************************

//Temp Innen

void drawBarTempInnen(int index, int value, int valueOffset){
  
  int maxValue = 100*2.05; //100; height factor
  
  // lower left corner coordinates for the bargrah
  int yaxis = 230; //Höhe
  int xaxis = 80; //Links/Rechts
  
  int margin = 2;
  int width = 12; //6;
  
   // given value range (after offset) is between 0 and 100 (950-1050)
  // and display height = 100, adjust to full display range
  
  int height = (value - valueOffset)*5.75; //Set height with * factor
    if (height < 1){
  height = 0;
  }
  int x1 = index * width;
  int x2 = x1 + width - margin;
 

  // draw it
  myGLCD.setColor(10,50,10);
  myGLCD.fillRect(xaxis + x1, yaxis, xaxis + x2, yaxis-height);
  
  // top very visible
  myGLCD.setColor(50,220,50);
  int markerHeight = 5;
  if (height < markerHeight){
    markerHeight = height;
  }
  myGLCD.fillRect(xaxis + x1, yaxis-height, xaxis + x2, yaxis-height+markerHeight);
  
  // clear top (to replace previous bar if it was higher)
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(xaxis + x1, yaxis - maxValue, xaxis + x2, yaxis-height);

}

void drawGraphTempInnen(int values[], int nr_of_values){
  
  for (int i = 0; i< nr_of_values; i++){
    drawBarTempInnen(i,values[i], 5); // value offset 5 gives us 0 in graph for 5°C
  }
}


