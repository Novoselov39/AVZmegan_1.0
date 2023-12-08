
 #define html0 "geotree.ru/coordinates?lat="
 #define html2 "&lon="
 #define html3 "&z=17&mlat"  

String gps_monitor(){
  float flat, flon;
  bool newData = false;
  String html1;
  String flat_gps, flon_gps;
  unsigned long chars;
  //unsigned long age;
  SIM900.end();
  
  gpsSerial.begin(9600);

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (gpsSerial.available())
    {
      char c = gpsSerial.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  if (newData)
  {
  
  //Serial.println(memoryFree());
  

  
  //Serial.println(memoryFree());
   gps.f_get_position(&flat, &flon);
  //gps.f_get_position(&flat, &flon, &age);
  flat_gps=String(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
  //smartdelay(0);
  flon_gps=String(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  }
  html1=html0+flat_gps+html2+flon_gps+html3;  
  //Serial.println(html1);

   
  gpsSerial.end();
  SIM900.begin(9600);
  sendATCommand("AT", true);  
  return html1;
  
  html1="";     
}


//// свободное память

extern int __bss_end;
extern void *__brkval;

int memoryFree()
{
   int freeValue;
   if((int)__brkval == 0)
      freeValue = ((int)&freeValue) - ((int)&__bss_end);
   else
      freeValue = ((int)&freeValue) - ((int)__brkval);
   return freeValue;
}
