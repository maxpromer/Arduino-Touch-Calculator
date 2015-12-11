#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SPFD5408_TouchScreen.h>

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

// Calibrate values
#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
// optional
#define LCD_RESET A4

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESSURE 10
#define MAXPRESSURE 1000

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

String Key[4][4] = {
  { "7", "8", "9", "/" },
  { "4", "5", "6", "*" },
  { "1", "2", "3", "-" },
  { "C", "0", "=", "+" }
};

String N1, N2, ShowSC, opt;
bool updata=false;
float answers=-1;

void setup() {
  Serial.begin(9600);
  tft.reset();
  tft.begin(0x9341); // SDFP5408
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  
  tft.fillRect(0, 80, 240, 240, WHITE);
  tft.drawFastHLine(0, 80, 240, BLACK);
  tft.drawFastHLine(0, 140, 240, BLACK);
  tft.drawFastHLine(0, 200, 240, BLACK);
  tft.drawFastHLine(0, 260, 240, BLACK);
  tft.drawFastHLine(0, 320-1, 240, BLACK);

  tft.drawFastVLine(0, 80, 240, BLACK);
  tft.drawFastVLine(60, 80, 240, BLACK);
  tft.drawFastVLine(120, 80, 240, BLACK);
  tft.drawFastVLine(180, 80, 240, BLACK);
  tft.drawFastVLine(240-1, 80, 240, BLACK);

  for (int y=0;y<4;y++) {
    for (int x=0;x<4;x++) {
      tft.setCursor(22 + (60*x), 100 + (60*y));
      tft.setTextSize(3);
      tft.setTextColor(BLACK);
      tft.println(Key[y][x]);
    }
  }
}

void loop() {
  TSPoint p = waitTouch();
  Serial.println(p.x);
  updata = false;
  for (int i1=0;i1<4;i1++) {
    for (int i2=0;i2<4;i2++) {
      if ((p.x>=240-((i1+1)*60)+1&&p.x<=240-(i1*60)-1)&&(p.y>=(i2*60)+1&&p.y<=((i2+1)*60)-1)) {
        if ((i1<=2&&i2<=2)||(i1==3&&i2==1)) {
          if (opt==0) {
            if (answers!=-1) answers = -1;
            N1 = N1 + Key[i1][i2];
            ShowSC = N1;
          } else {
            N2 = N2 + Key[i1][i2];
            ShowSC = opt + N2;
          }
        } else {
          if (Key[i1][i2]=="C") {
            N1 = N2 = "";
            opt = "";
            answers = 0;
            ShowSC = N1;
          } else if (i2==3) {
            if (N1=="") N1 = String(answers);
            opt = Key[i1][i2];
            ShowSC = Key[i1][i2];
          } else if (Key[i1][i2]=="=") {
            if (opt=="+") answers = N1.toInt() + N2.toInt();
            else if (opt=="-") answers = N1.toInt() - N2.toInt();
            else if (opt=="*") answers = N1.toInt() * N2.toInt();
            else if (opt=="/") answers = N1.toInt() / N2.toInt();
            N1 = N2 = opt = "";
            ShowSC = answers;
          }
        }
        updata = true;
      }
    }
  }
  
  if (updata) {
    tft.fillRect(0, 0, 240, 80, BLACK);
  
    tft.setCursor(10, 10);
    tft.setTextSize(3);
    tft.setTextColor(WHITE);
    tft.println(ShowSC);
  }
  delay(300);
}

TSPoint waitTouch() {
  TSPoint p;
  do {
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 240);;
  return p;
}

