
#define DEBUG5//test new feature

void setup()
{
  // Initialize the debug serial port
  Serial.begin(9600); 
  
    CharDisplayInit();
    delay (200);

  Serial.println ("Test");
  #if defined DEBUG5//HACK
    CharDisplayPrintLine(1, "Looking for ");
    CharDisplayPrintLine(2, "DEBUG CASE");
    Serial.println (" ");
  #else
    CharDisplayPrintLine(1, "NOKLJDSFKLSFJLKDSF ");
    CharDisplayPrintLine(2, "else case");
    Serial.print  ("NOKLJDSFKLSFJLKDSF ");
    Serial.println  ("NOKLJDSFKLSJLKDFSJLKDSF");
   #endif
  Serial.println ("WAIT HERE!!!");
  delay (4000);
}
 void loop()
{
 
}

void CharDisplayPrintLine(int line, const char* strText)
{
  uint8_t lineCode = 128;
  if (line == 2) lineCode = 192;

  // Force a field width of 16.
  // Left justified with space padding on the right.
  char strBuffer[32];
  sprintf(strBuffer, "%-16.16s",  strText);

  // For debug -- prints what goes to the display
  //Serial.print("[");
  //Serial.print(strBuffer);
  //Serial.println("]");
  
  Serial1.write((uint8_t)254); 
  Serial1.write(lineCode);  
  Serial1.write(strBuffer, 16);
}

void CharDisplayInit()
{
  Serial1.begin(9600);
  delay(500);
  
  // Clear the screen
  Serial1.write((uint8_t)254);
  Serial1.write((uint8_t)1);

  // Move cursor to beginning of line1
  Serial1.write(254); 
  Serial1.write(128);  
}
