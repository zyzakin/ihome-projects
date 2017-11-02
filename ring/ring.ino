    #include <Adafruit_NeoPixel.h>
     
    #define PIN 5
    #define LICZBADIOD 16

    int r, g, b;
     
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LICZBADIOD, PIN, NEO_GRB + NEO_KHZ800);
     
    void setup()
    {
      
      pixels.begin(); // Inicjalizacja biblioteki

    }
     
    void loop()
    {
      
      
      for(int i=0; i<LICZBADIOD; i++)
      {
        pixels.setPixelColor(i, 130, 0, 0); // Dioda "i" oraz skladowe R=255 G=0 B=0
        pixels.show(); // Wysylamy dane do lancucha
        delay(200); // Opoznienie 500ms przed zaswieceniem kolejnej diody
      }
      
      for(int i=0; i<LICZBADIOD; i++)
      {
        pixels.setPixelColor(i, 0, 100, 0); // Dioda "i" oraz skladowe R=255 G=0 B=0
        pixels.show(); // Wysylamy dane do lancucha
        delay(200); // Opoznienie 500ms przed zaswieceniem kolejnej diody
      }

    }

