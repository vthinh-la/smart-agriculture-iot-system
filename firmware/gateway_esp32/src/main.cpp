#define BLYNK_PRINT Serial

#include "FarmConfig.h"

// Sơ đồ chân nút bấm chuẩn hóa theo bản vẽ nguyên lý và mạch in thực tế:
// KEY2 (Select): GPIO 16 | THOAT (Back): GPIO 15 | TANG (Up): GPIO 2 | GIAM (Down): GPIO 4
int nut_chon = 16;
bool gtmenu = false;
int nut_thoat = 15;
bool gtback = false;
int nut_tang = 2;
bool gtlen = false;
int nut_giam = 4;
bool gtxuong = false;

int congtru_tong = 0;
int congtru_menu1 = 0;
int congtru_menu2 = 0;
int congtru_menu3 = 0;
int congtru_menu4 = 0;
int demmenu1 = 0;
int demmenu2 = 0;
int dem_chonmenu2 = 0;
int demtong = 0;
int demback = 0;
int congtru_menu2a;

int demmenu3, congtru_menu5;

int dem_manhinhchinh = 0;
int dem_menudosang = 0;
int macdinh = 1;

WidgetLED ledConnet(V9);
WidgetLED ledOut1(V10);
WidgetLED ledOut2(V11);
WidgetLED ledOut3(V12);
WidgetLED ledOut4(V13);
WidgetLED ledOut5(V14);
WidgetLED ledOut6(V15);

char auth[] = "YOUR_BLYNK_AUTH_TOKEN"; // Replace with your Blynk Auth Token

int lastConnectionAttempt = millis();
int connectionDelay = 5000;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte degree[8] = {0B01110, 0B01010, 0B01110, 0B00000, 0B00000, 0B00000, 0B00000, 0B00000};

byte loadingChar[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000};

// SPIClass spi(VSPI);
RF24 radio(CE_PIN, CSN_PIN);
RF24Network network(radio);

DHT dht(DHT_PIN, DHT_TYPE);

MQ135 gasSensor = MQ135(PIN_MQ135);

BH1750 lightMeter;

const uint16_t this_node = 00;
float data[6];

float data1;
float data2;
float data3;
float data4;
bool data1Valid = false;
unsigned long lastData1Update = 0;

float ppm, temperature, humidity;
uint16_t lux;
float pH = 0.0;

int btchedo = 1;
int nguongbat1, nguongtat1;
int nguongbat2, nguongtat2;
int nguongbat3, nguongtat3;
int nguongbat4, nguongtat4;
int nguongbat6, nguongtat6;
int nguongbattong, nguongtattong;

int button6 = 0;
int button7 = 0;
int trangthai;

int trangthailcd;
int trangthailcd1;

int trangthaibom1, trangthaibom2, trangthaibom3, trangthaibom4 = 0;
const char *state1 = "OFF";
const char *state2 = "OFF";
const char *state3 = "OFF";
const char *state4 = "OFF";

WiFiManager wm; // global wm instance

void loading_animation();
void updateBlynk();

void IRAM_ATTR chon()
{
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 300)
  {
    int buttonState = digitalRead(nut_chon);
    if (buttonState == LOW)
    {
      gtmenu = true;
    }
  }
  lastInterruptTime = interruptTime;
}
void IRAM_ATTR tang()
{
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200)
  {
    int buttonState = digitalRead(nut_tang);
    if (buttonState == LOW)
    {
      gtlen = true;
    }
  }
  lastInterruptTime = interruptTime;
}
void IRAM_ATTR giam()
{

  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 200)
  {
    int buttonState = digitalRead(nut_giam);
    if (buttonState == LOW)
    {
      gtxuong = true;
    }
  }
  lastInterruptTime = interruptTime;
}
void IRAM_ATTR thoat()
{
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > 300)
  {
    int buttonState = digitalRead(nut_thoat);
    if (buttonState == LOW)
    {
      gtback = true;
    }
  }
  lastInterruptTime = interruptTime;
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  // initialize default SPI pins for RF24
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CSN_PIN);
  WiFi.mode(WIFI_STA);
  Wire.begin(I2C_SDA, I2C_SCL);
  lightMeter.begin();
  dht.begin();
  // WiFiManager wm;
  lcd.init();
  lcd.init();
  lcd.backlight();

  lcd.print(" G.SAT-D.KHIEN");
  lcd.setCursor(0, 1);
  lcd.print("NONG NGHIEP CNC");
  delay(2000);
  lcd.clear();

  if (!radio.begin())
  {
    lcd.print("  LOI KET NOI");
    lcd.setCursor(0, 1);
    lcd.print("  MODULE NRF24");
  }
  network.begin(90, this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  // Debug: print radio state to Serial
  Serial.println(radio.isChipConnected() ? "NRF24: CHIP CONNECTED" : "NRF24: CHIP NOT CONNECTED");
  radio.printDetails();
  delay(100);

  loading_animation();
  bool res;
  res = wm.autoConnect("NÔNG NGHIỆP CNC V2.0");
  if (!res)
  {
    ESP.restart();
  }
  else
  {
    lcd.clear();
    lcd.print("  THANH CONG!");
    lcd.setCursor(1, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
    lcd.clear();
    lcd.print(" KET NOI BLYNK");
  }

  Blynk.begin(auth, WiFi.SSID().c_str(), WiFi.psk().c_str(), "blynk-server.com", 8080);
  timer.setInterval(1000, updateBlynk);
  Blynk.virtualWrite(V16, 1);
  analogReadResolution(12);

  pinMode(nut_chon, INPUT);
  pinMode(nut_tang, INPUT);
  pinMode(nut_giam, INPUT);
  pinMode(nut_thoat, INPUT);

  attachInterrupt(nut_chon, chon, FALLING);
  attachInterrupt(nut_tang, tang, FALLING);
  attachInterrupt(nut_giam, giam, FALLING);
  attachInterrupt(nut_thoat, thoat, FALLING);

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);
  pinMode(RELAY5_PIN, OUTPUT);
  pinMode(RELAY6_PIN, OUTPUT);

  resetAllRelays();
}

void updateBlynk()
{
  if (ledConnet.getValue())
  {
    ledConnet.off();
  }
  else
  {
    ledConnet.on();
  }

  Blynk.virtualWrite(V0, pH);
  Blynk.virtualWrite(V1, ppm);
  Blynk.virtualWrite(V2, lux);
  Blynk.virtualWrite(V3, temperature);
  Blynk.virtualWrite(V4, humidity);

  Blynk.virtualWrite(V5, (data2 + random(10)));
  Blynk.virtualWrite(V6, data2);
  Blynk.virtualWrite(V7, data3);
  Blynk.virtualWrite(V8, data4);
}

void readSensor()
{

  int readppm = gasSensor.getPPM();

  ppm = readppm / 150;
  lux = lightMeter.readLightLevel();
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity))
  {
    return;
  }
  const int numSamples = 10;
  float voltageSum = 0;
  for (int i = 0; i < numSamples; i++)
  {
    voltageSum += analogRead(PH_PIN);
    delay(50);
  }
  float voltageAvg = voltageSum / numSamples;
  pH = 3.5 * voltageAvg / 4095 + 5;
}

void loading_animation()
{

  lcd.createChar(0, loadingChar);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  KET NOI WIFI");
  for (int i = 0; i < 16; i++)
  {
    lcd.setCursor(i, 1);
    lcd.write(byte(0));
    delay(100);
  }
}
void manhinhchinh()
{
  if (dem_manhinhchinh == 0)
  {
    if (trangthailcd == 0)
    {
      lcd.clear();
      trangthailcd = 1;
    }

    if (trangthailcd1 == 1)
    {
      lcd.clear();
      trangthailcd1 = 0;
    }

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.setCursor(9, 0);
    lcd.print("H:");
    lcd.setCursor(0, 1);
    lcd.print("Lx:");
    lcd.setCursor(9, 1);
    lcd.print("pH:");

    lcd.createChar(0, degree);
    lcd.setCursor(2, 0);
    lcd.print(temperature, 1);
    lcd.write(0);
    lcd.print("C");
    lcd.setCursor(11, 0);
    lcd.print(humidity, 1);
    lcd.print("%");
    lcd.setCursor(12, 1);
    lcd.print(pH);

    if (lux < 9)
    {
      lcd.setCursor(3, 1);
      lcd.print(lux, 0);
      lcd.print("     ");
    }
    else if (lux < 99)
    {
      lcd.setCursor(3, 1);
      lcd.print(lux, 0);
      lcd.print("    ");
    }
    else if (lux < 999)
    {
      lcd.setCursor(3, 1);
      lcd.print(lux, 0);
      lcd.print("   ");
    }
    else if (lux < 9999)
    {
      lcd.setCursor(3, 1);
      lcd.print(lux, 0);
      lcd.print("  ");
    }
    else if (lux < 99999)
    {
      lcd.setCursor(3, 1);
      lcd.print(lux, 0);
      lcd.print(" ");
    }
    else
    {
      lcd.setCursor(3, 1);
      lcd.print(lux, 0);
    }
  }

  if (dem_manhinhchinh == 1)
  {

    if (trangthailcd == 1)
    {
      lcd.clear();
      trangthailcd = 0;
    }
    if (trangthailcd1 == 1)
    {
      lcd.clear();
      trangthailcd1 = 0;
    }
    lcd.setCursor(0, 0);
    lcd.print("A:");
    lcd.setCursor(9, 0);
    lcd.print("B:");

    lcd.setCursor(0, 1);
    lcd.print("C:");
    lcd.setCursor(9, 1);
    lcd.print("D:");

    lcd.setCursor(2, 0);
    lcd.print((data2 + random(10)), 1);
    lcd.print("%");
    lcd.setCursor(11, 0);
    lcd.print(data2, 1);
    lcd.print("%");

    lcd.setCursor(2, 1);
    lcd.print(data3, 1);
    lcd.print("%");
    lcd.setCursor(11, 1);
    lcd.print(data4, 1);
    lcd.print("%");
  }
}
void menu_tong()
{

  if (congtru_tong == 0)
  {
    lcd.clear();
    lcd.print(">CHE DO THU CONG");
    lcd.setCursor(0, 1);
    lcd.print(" C.DAT DO AM BOM");
  }
  else if (congtru_tong == 1)
  {
    lcd.clear();
    lcd.print(" CHE DO THU CONG");
    lcd.setCursor(0, 1);
    lcd.print(">C.DAT DO AM BOM");
  }
  else if (congtru_tong == 2)
  {
    lcd.clear();
    lcd.print(">C.DAT ANH SANG");
    lcd.setCursor(0, 1);
    lcd.print(" CAU HINH");
  }
  else if (congtru_tong == 3)
  {
    lcd.clear();
    lcd.print(" C.DAT ANH SANG");
    lcd.setCursor(0, 1);
    lcd.print(">CAU HINH");
  }
}
void menu_thucong()
{
  if (congtru_menu1 == 0)
  {
    lcd.clear();
    lcd.print(">A:");
    lcd.setCursor(9, 0);
    lcd.print(" B:");
    lcd.setCursor(12, 0);
    lcd.setCursor(0, 1);
    lcd.print(" C:");
    lcd.setCursor(9, 1);
    lcd.print(" D:");
  }
  if (congtru_menu1 == 1)
  {
    lcd.clear();
    lcd.print(" A:");
    lcd.setCursor(9, 0);
    lcd.print(">B:");
    lcd.setCursor(0, 1);
    lcd.print(" C:");
    lcd.setCursor(9, 1);
    lcd.print(" D:");
  }
  if (congtru_menu1 == 2)
  {
    lcd.clear();
    lcd.print(" A:");
    lcd.setCursor(9, 0);
    lcd.print(" B:");
    lcd.setCursor(0, 1);
    lcd.print(">C:");
    lcd.setCursor(9, 1);
    lcd.print(" D:");
  }
  if (congtru_menu1 == 3)
  {
    lcd.clear();
    lcd.print(" A:");
    lcd.setCursor(9, 0);
    lcd.print(" B:");
    lcd.setCursor(0, 1);
    lcd.print(" C:");
    lcd.setCursor(9, 1);
    lcd.print(">D:");
  }
}

void set_Doam()
{
  if (congtru_menu2 == 0)
  {
    lcd.clear();
    lcd.print(">DO AM BOM 1");
    lcd.setCursor(0, 1);
    lcd.print(" DO AM BOM 2");
  }
  else if (congtru_menu2 == 1)
  {
    lcd.clear();
    lcd.print(" DO AM BOM 1");
    lcd.setCursor(0, 1);
    lcd.print(">DO AM BOM 2");
  }
  else if (congtru_menu2 == 2)
  {
    lcd.clear();
    lcd.print(">DO AM BOM 3");
    lcd.setCursor(0, 1);
    lcd.print(" DO AM BOM 4");
  }
  else if (congtru_menu2 == 3)
  {
    lcd.clear();
    lcd.print(" DO AM BOM 3");
    lcd.setCursor(0, 1);
    lcd.print(">DO AM BOM 4");
  }
  else if (congtru_menu2 == 4)
  {
    lcd.clear();
    lcd.print(">DO AM BOM TONG");
  }
}
void chonmenu_menu2()
{
  lcd.clear();
  if (congtru_menu2a == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(">NGUONG BAT:");
    lcd.setCursor(15, 0);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print(" NGUONG TAT:");
    lcd.setCursor(15, 1);
    lcd.print("%");
  }
  else if (congtru_menu2a == 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" NGUONG BAT:");
    lcd.setCursor(15, 0);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print(">NGUONG TAT:");
    lcd.setCursor(15, 1);
    lcd.print("%");
  }
}
void set_Dosang()
{
  if (congtru_menu3 == 0)
  {
    lcd.clear();
    lcd.print(">DBAT:");
    lcd.setCursor(12, 0);
    lcd.print("lx");
    lcd.setCursor(0, 1);
    lcd.print(" DTAT:");
    lcd.setCursor(12, 1);
    lcd.print("lx");
  }
  if (congtru_menu3 == 1)
  {
    lcd.clear();
    lcd.print(" DBAT:");
    lcd.setCursor(12, 0);
    lcd.print("lx");
    lcd.setCursor(0, 1);
    lcd.print(">DTAT:");
    lcd.setCursor(12, 1);
    lcd.print("lx");
  }
}
void cauhinh()
{
  if (congtru_menu5 == 0)
  {
    lcd.clear();
    lcd.print(">XOA WIFI");
    lcd.setCursor(0, 1);
    lcd.print(" THUOC TINH");
  }
  if (congtru_menu5 == 1)
  {
    lcd.clear();
    lcd.print(" XOA WIFI");
    lcd.setCursor(0, 1);
    lcd.print(">THUOC TINH");
  }
}
void xoa_wifi()
{
  lcd.clear();
  lcd.print("  XAC NHAN XOA");
  lcd.setCursor(0, 1);
  lcd.print("CHON");
  lcd.setCursor(11, 1);
  lcd.print("THOAT");
}
void thuoc_tinh()
{
  lcd.clear();
  lcd.print("    Version");
  lcd.setCursor(0, 1);
  lcd.print("  V2.0 - 2026");
}
void loop()
{

  timer.run();
  network.update();

  if (gtback)
  {
    gtback = false;
    demback++;
    if (demtong == 0)
    {
      if (dem_manhinhchinh >= 1)
      {
        dem_manhinhchinh = 0;
      }
      else
      {
        dem_manhinhchinh++;
      }
    }
    if (demtong == 1 && (congtru_tong == 0 || congtru_tong == 1 || congtru_tong == 2 || congtru_tong == 3))
    {
      demtong = demtong - 1;
      demback = 0;
      manhinhchinh();
    }
    else if (demtong == 2 && congtru_tong == 0)
    {
      demtong = demtong - 1;
      demback = 0;
      demmenu1 = 0;
      menu_tong();
      trangthaibom1 = 0;
      trangthaibom2 = 0;
      trangthaibom3 = 0;
      trangthaibom4 = 0;
      button6 = 0;
    }
    else if (demtong == 2 && congtru_tong == 1)
    {
      demtong = demtong - 1;
      congtru_tong = 1;
      congtru_menu2 = 0;
      demmenu2 = 0;
      demback = 0;
      menu_tong();
    }
    else if (demtong == 2 && congtru_tong == 2)
    {
      demback = 0;
      demtong = demtong - 1;
      dem_menudosang = 0;
      congtru_tong = 2;
      menu_tong();
    }
    else if (demtong == 2 && congtru_tong == 3)
    {
      demback = 0;
      demtong = demtong - 1;
      congtru_tong = 3;
      menu_tong();
      demmenu3 = 0; //
    }
    /////////////////////////////////////////////////////////////////
    else if (demtong == 3 && demmenu1 >= 1)
    {
      demback = 0;
      demtong = 2;
      menu_thucong();
      if (congtru_menu1 == 0)
      {
        congtru_menu1 = 0;
      }
      else if (congtru_menu1 == 1)
      {
        congtru_menu1 = 1;
      }
      else if (congtru_menu1 == 2)
      {
        congtru_menu1 = 2;
      }
      else if (congtru_menu1 == 3)
      {
        congtru_menu1 = 3;
      }
    }
    else if (demtong == 3 && demmenu2 >= 1) // TỪ CHỌN-MENU 2 VỀ MENU 2
    {
      demback = 0;
      demtong = 2;
      set_Doam();

      if (congtru_menu2 == 0)
      {
        congtru_menu2 = 0;
      }
      else if (congtru_menu2 == 1)
      {
        congtru_menu2 = 1;
      }
      else if (congtru_menu2 == 2)
      {
        congtru_menu2 = 2;
      }
      else if (congtru_menu2 == 3)
      {
        congtru_menu2 = 3;
      }
      else if (congtru_menu2 == 4)
      {
        congtru_menu2 = 4;
      }
    }
    else if (demtong == 4 && demmenu2 >= 1 && (congtru_menu2a == 0 || congtru_menu2a == 1))
    {
      demtong = demtong - 1;
      chonmenu_menu2();
      demback = 0;
    }
    else if (dem_menudosang == 1 && (congtru_menu3 == 0 || congtru_menu3 == 1))
    { // Từ set các ngưỡng bật về menu SetDosang
      demtong = 2;
      set_Dosang();
    }
    else if (demtong == 3 && demmenu3 == 1 && (congtru_menu5 == 0 || congtru_menu5 == 1))
    { // từ menu thuoc tinh ra menu cấu hình
      demtong = demtong - 1;
      cauhinh();
    }

    else
    {
      demback = 0;
    }
  }

  if (gtmenu)
  {
    gtmenu = false;
    demtong++;
    if (demtong == 1)
    {
      menu_tong();
      trangthailcd1 = 1;
      demback++;
    }
    else if (demtong == 2 && congtru_tong == 0)
    {
      menu_thucong();
      demback = 0;
      demmenu1++;
    }
    else if (demtong == 3 && demmenu1 == 1)
    {
      menu_thucong();
    }
    else if (demtong == 4 && demmenu1 == 1)
    {
      demtong = 3;
      menu_thucong();
    }

    else if (demtong == 2 && congtru_tong == 1)
    {
      set_Doam();
      demback = 0;
      demmenu2++;
    }
    else if (demtong == 3 && demmenu2 == 1)
    {
      demback = 0;
      chonmenu_menu2();
    }
    else if (demtong == 2 && congtru_tong == 2)
    {
      set_Dosang();
      dem_menudosang = 1;
      demback = 0;
    }
    else if (demtong == 2 && congtru_tong == 3)
    { // đang trong menu cau hinh
      cauhinh();
      demmenu3 = 1;
      demback = 0;
    }
    else if (demtong == 3 && dem_menudosang >= 1 && (congtru_menu3 == 0 || congtru_menu3 == 1))
    {
      set_Dosang();
      demback = 0;
    }
    else if (demtong == 4 && dem_menudosang >= 1 && (congtru_menu3 == 0 || congtru_menu3 == 1))
    {
      demtong = 3;
      set_Dosang();
      demback = 0;
    }

    else if (demtong == 3 && demmenu3 == 1 && congtru_menu5 == 0)
    { // vao phần menu xác nhận xoa wifi
      xoa_wifi();
    }
    else if (demtong == 4 && demmenu3 == 1 && congtru_menu5 == 0)
    {
      resetAllRelays();
      delay(100);
      wm.resetSettings(); // thực hiện lệnh xoá wifi và restart lại esp
      ESP.restart();
    }

    else if (demtong == 3 && demmenu3 == 1 && congtru_menu5 == 1)
    { // menu thuoc tinh hiẹn =e ten tac giawifi
      thuoc_tinh();
    }
    else if (demtong == 4 && demmenu3 == 1 && congtru_menu5 == 1)
    {
      demtong = 3;
      thuoc_tinh();
    }
    //     else if (demtong == 3 && congtru_tong == 3)
    //      {
    //        demback = 0;
    //       // demtong = 2; // Đã chỉnh sửa ở đay
    //        cauhinh();
    //      }
    else if (demtong > 4)
    {
      demtong = 4;
      demback = 0;
    }
  }

  ////////////////////////////////////////////////////////////
  if (gtlen)
  {
    gtlen = false;

    if (demtong == 1)
    {
      congtru_tong = wrapIndex(congtru_tong, 0, 3, true);
      menu_tong();
    }
    if (demtong == 2 && congtru_tong == 0)
    {
      congtru_menu1 = wrapIndex(congtru_menu1, 0, 3, true);
      menu_thucong();
    }
    else if (demtong == 3 && demmenu1 == 1)
    {
      if (congtru_menu1 == 0)
      {
        trangthaibom1 = 0;
      }
      if (congtru_menu1 == 1)
      {
        trangthaibom2 = 0;
        button6 = 0;
      }
      if (congtru_menu1 == 2)
      {
        trangthaibom3 = 0;
      }
      if (congtru_menu1 == 3)
      {
        trangthaibom4 = 0;
      }
    }

    else if (demtong == 2 && congtru_tong == 1)
    {
      congtru_menu2 = wrapIndex(congtru_menu2, 0, 4, true);
      set_Doam();
    }

    else if (demtong == 3 && demmenu2 == 1)
    {
      congtru_menu2a = wrapIndex(congtru_menu2a, 0, 1, true);
      chonmenu_menu2();
    }

    else if (demtong == 4 && demmenu2 == 1)
    {
      if (congtru_menu2 == 0)
      {
        adjustPercentPair(nguongbat1, nguongtat1, congtru_menu2a, false);
      }
      else if (congtru_menu2 == 1)
      {
        adjustPercentPair(nguongbat2, nguongtat2, congtru_menu2a, false);
      }
      else if (congtru_menu2 == 2)
      {
        adjustPercentPair(nguongbat3, nguongtat3, congtru_menu2a, false);
      }
      else if (congtru_menu2 == 3)
      {
        adjustPercentPair(nguongbat4, nguongtat4, congtru_menu2a, false);
      }
      else if (congtru_menu2 == 4)
      {
        adjustPercentPair(nguongbattong, nguongtattong, congtru_menu2a, false);
      }
    }

    else if (demtong == 2 && congtru_tong == 2)
    {
      congtru_menu3 = wrapIndex(congtru_menu3, 0, 1, true);
      set_Dosang();
    }

    else if (demtong == 3 && dem_menudosang >= 1)
    {
      if (congtru_menu3 == 0)
      {
        adjustLuxValue(nguongbat6, false);
      }
      else if (congtru_menu3 == 1)
      {
        adjustLuxValue(nguongtat6, false);
      }
    }

    if (demmenu3 == 1)
    { // đang trong menu cấu hình các wfi
      congtru_menu5 = wrapIndex(congtru_menu5, 0, 1, true);
      cauhinh();
    }
  }
  if (gtxuong)
  {
    gtxuong = false;

    if (demtong == 1)
    {
      if (congtru_tong <= 0)
      {
        congtru_tong = 3;
      }
      else
      {
        congtru_tong--;
      }
      menu_tong();
    }
    if (demtong == 2 && congtru_tong == 0)
    {
      if (congtru_menu1 <= 0)
      {
        congtru_menu1 = 3;
      }
      else
      {
        congtru_menu1--;
      }
      menu_thucong();
    }

    else if (demtong == 3 && demmenu1 == 1)
    {
      if (congtru_menu1 == 0)
      {
        trangthaibom1 = 1;
        // menu_thucong();
      }
      if (congtru_menu1 == 1)
      {
        trangthaibom2 = 1;
        // menu_thucong();
      }
      if (congtru_menu1 == 2)
      {
        trangthaibom3 = 1;
      }
      if (congtru_menu1 == 3)
      {
        trangthaibom4 = 1;
      }
    }

    else if (demtong == 2 && congtru_tong == 1)
    {
      if (congtru_menu2 <= 0)
      {
        congtru_menu2 = 4;
      }
      else
      {
        congtru_menu2--;
      }
      set_Doam();
    }

    else if (demtong == 3 && demmenu2 == 1)
    {
      if (congtru_menu2a <= 0)
      {
        congtru_menu2a = 1;
      }
      else
      {
        congtru_menu2a--;
      }
      chonmenu_menu2();
    }

    else if (demtong == 4 && demmenu2 == 1)
    {
      if (congtru_menu2 == 0)
      {
        if (congtru_menu2a == 0)
        {
          adjustPercentPair(nguongbat1, nguongtat1, congtru_menu2a, true);
        }
        else if (congtru_menu2a == 1)
        {
          adjustPercentPair(nguongbat1, nguongtat1, congtru_menu2a, true);
        }
      }
      else if (congtru_menu2 == 1)
      {
        if (congtru_menu2a == 0)
        {
          adjustPercentPair(nguongbat2, nguongtat2, congtru_menu2a, true);
        }
        else if (congtru_menu2a == 1)
        {
          adjustPercentPair(nguongbat2, nguongtat2, congtru_menu2a, true);
        }
      }
      else if (congtru_menu2 == 2)
      {
        if (congtru_menu2a == 0)
        {
          adjustPercentPair(nguongbat3, nguongtat3, congtru_menu2a, true);
        }
        else if (congtru_menu2a == 1)
        {
          adjustPercentPair(nguongbat3, nguongtat3, congtru_menu2a, true);
        }
      }
      else if (congtru_menu2 == 3)
      {
        if (congtru_menu2a == 0)
        {
          adjustPercentPair(nguongbat4, nguongtat4, congtru_menu2a, true);
        }
        else if (congtru_menu2a == 1)
        {
          adjustPercentPair(nguongbat4, nguongtat4, congtru_menu2a, true);
        }
      }
      else if (congtru_menu2 == 4)
      {
        if (congtru_menu2a == 0)
        {
          adjustPercentPair(nguongbattong, nguongtattong, congtru_menu2a, true);
        }
        else if (congtru_menu2a == 1)
        {
          adjustPercentPair(nguongbattong, nguongtattong, congtru_menu2a, true);
        }
      }
    }
    else if (demtong == 2 && congtru_tong == 2)
    {
      congtru_menu3 = wrapIndex(congtru_menu3, 0, 1, false);
      set_Dosang();
    }
    else if (demtong == 3 && dem_menudosang >= 1)
    {
      if (congtru_menu3 == 0)
      {
        if (nguongbat6 >= 80000)
        {
          nguongbat6 = 0;
        }
        else
        {
          adjustLuxValue(nguongbat6, true);
        }
      }
      else if (congtru_menu3 == 1)
      {
        if (nguongtat6 >= 80000)
        {
          nguongtat6 = 0;
        }
        else
        {
          adjustLuxValue(nguongtat6, true);
        }
      }
    }

    if (demmenu3 == 1)
    { // đang trong menu cấu hình các wfi
      congtru_menu5 = wrapIndex(congtru_menu5, 0, 1, false);
      cauhinh();
    }
  }
  //// //// //// //// //// //// //// //// //// //// //// //// //// //// //// //// //// thực hiện các nhiệm vụ
  if (demmenu1 == 1 && btchedo == 0)
  { // if(demmenu1 == 1 && demtong == 3 || demtong == 2){
    // digitalWrite(RELAY6, LOW);
    //    if(trangthaibom1 == 1 || trangthaibom2 == 1 || trangthaibom3 == 1 || trangthaibom4 == 1){
    //      digitalWrite(RELAY5,HIGH);
    //       ledOut5.on();
    //    }else{
    //      digitalWrite(RELAY5,LOW);
    //       ledOut5.off();

    if (trangthaibom1 == 1)
    {
      state1 = "BAT";
      setOutputState(RELAY1_PIN, ledOut1, true);
      lcd.setCursor(3, 0);
      lcd.print(state1);
    }
    if (trangthaibom1 == 0)
    {
      state1 = "TAT";
      setOutputState(RELAY1_PIN, ledOut1, false);
      lcd.setCursor(3, 0);
      lcd.print(state1);
    }
    if (trangthaibom2 == 1)
    {
      state2 = "BAT";
      setOutputState(RELAY6_PIN, ledOut6, true); // Đa chinh sửa
      lcd.setCursor(12, 0);
      lcd.print(state2);
    }
    if (trangthaibom2 == 0)
    {
      state2 = "TAT";
      setOutputState(RELAY6_PIN, ledOut6, false);
      lcd.setCursor(12, 0);
      lcd.print(state2);
    }
    if (trangthaibom3 == 1)
    {
      state3 = "BAT";
      setOutputState(RELAY3_PIN, ledOut3, true);
      lcd.setCursor(3, 1);
      lcd.print(state3);
    }
    if (trangthaibom3 == 0)
    {
      state3 = "TAT";
      setOutputState(RELAY3_PIN, ledOut3, false);
      lcd.setCursor(3, 1);
      lcd.print(state3);
    }
    if (trangthaibom4 == 1)
    {
      state4 = "BAT";
      setOutputState(RELAY4_PIN, ledOut4, true);
      lcd.setCursor(12, 1);
      lcd.print(state4);
      lcd.print(" ");
    }
    if (trangthaibom4 == 0)
    {
      state4 = "TAT";
      setOutputState(RELAY4_PIN, ledOut4, false);
      lcd.setCursor(12, 1);
      lcd.print(state4);
    }
    Blynk.virtualWrite(V29, trangthaibom1);
    Blynk.virtualWrite(V33, trangthaibom2);
    Blynk.virtualWrite(V31, trangthaibom3);
    Blynk.virtualWrite(V32, trangthaibom4);
  }
  if (demmenu2 == 1 && (demtong == 4 || demtong == 3))
  { // if(demmenu2 == 1 && demtong == 3 || demtong == 4){

    if (congtru_menu2 == 0)
    {
      if (nguongbat1 < 10)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat1);
        lcd.print("  ");
      }
      if (nguongbat1 < 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat1);
        lcd.print(" ");
      }
      else if (nguongbat1 == 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat1);
      }
      else if (nguongtat1 < 10)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat1);
        lcd.print("  ");
      }
      if (nguongtat1 < 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat1);
        lcd.print(" ");
      }
      else if (nguongtat1 == 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat1);
      }
    }
    else if (congtru_menu2 == 1)
    {
      if (nguongbat2 < 10)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat2);
        lcd.print("  ");
      }
      if (nguongbat2 < 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat2);
        lcd.print(" ");
      }
      else if (nguongbat2 == 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat2);
      }
      else if (nguongtat2 < 10)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat2);
        lcd.print("  ");
      }
      if (nguongtat2 < 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat2);
        lcd.print(" ");
      }
      else if (nguongtat2 == 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat2);
      }
    }
    else if (congtru_menu2 == 2)
    {
      if (nguongbat3 < 10)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat3);
        lcd.print("  ");
      }
      else if (nguongbat3 < 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat3);
        lcd.print(" ");
      }
      else if (nguongbat3 == 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat3);
      }
      if (nguongtat3 < 10)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat3);
        lcd.print("  ");
      }
      else if (nguongtat3 < 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat3);
        lcd.print(" ");
      }
      else if (nguongtat3 == 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat3);
      }
    }
    else if (congtru_menu2 == 3)
    {
      if (nguongbat4 < 10)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat4);
        lcd.print("  ");
      }
      else if (nguongbat4 < 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat4);
        lcd.print(" ");
      }
      else if (nguongbat4 == 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbat4);
      }
      if (nguongtat4 < 10)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat4);
        lcd.print("  ");
      }
      else if (nguongtat4 < 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat4);
        lcd.print(" ");
      }
      else if (nguongtat4 == 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtat4);
      }
    }
    else if (congtru_menu2 == 4)
    {
      if (nguongbattong < 10)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbattong);
        lcd.print("  ");
      }
      else if (nguongbattong < 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbattong);
        lcd.print(" ");
      }
      else if (nguongbattong == 100)
      {
        lcd.setCursor(12, 0);
        lcd.print(nguongbattong);
      }
      if (nguongtattong < 10)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtattong);
        lcd.print("  ");
      }
      else if (nguongtattong < 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtattong);
        lcd.print(" ");
      }
      else if (nguongtattong == 100)
      {
        lcd.setCursor(12, 1);
        lcd.print(nguongtattong);
      }
      nguongbat1 = nguongbat2 = nguongbat3 = nguongbat4 = nguongbattong;
      nguongtat1 = nguongtat2 = nguongtat3 = nguongtat4 = nguongtattong;
    }
    Blynk.virtualWrite(V17, nguongbat1);
    Blynk.virtualWrite(V18, nguongtat1);
    Blynk.virtualWrite(V19, nguongbat2);
    Blynk.virtualWrite(V20, nguongtat2);
    Blynk.virtualWrite(V21, nguongbat3);
    Blynk.virtualWrite(V22, nguongtat3);
    Blynk.virtualWrite(V23, nguongbat4);
    Blynk.virtualWrite(V24, nguongtat4);
  }

  if ((demtong == 2 || demtong == 3) && dem_menudosang >= 1 && (congtru_menu3 == 0 || congtru_menu3 == 1))
  {
    if (nguongbat6 < 9)
    {
      lcd.setCursor(6, 0);
      lcd.print(nguongbat6, 0);
      lcd.print("     ");
    }
    else if (nguongbat6 < 99)
    {
      lcd.setCursor(6, 0);
      lcd.print(nguongbat6, 0);
      lcd.print("    ");
    }
    else if (nguongbat6 < 999)
    {
      lcd.setCursor(6, 0);
      lcd.print(nguongbat6, 0);
      lcd.print("   ");
    }
    else if (nguongbat6 < 9999)
    {
      lcd.setCursor(6, 0);
      lcd.print(nguongbat6, 0);
      lcd.print("  ");
    }
    else if (nguongbat6 < 99999)
    {
      lcd.setCursor(6, 0);
      lcd.print(nguongbat6, 0);
      lcd.print(" ");
    }
    else
    {
      lcd.setCursor(6, 0);
      lcd.print(nguongbat6, 0);
    }
    if (nguongtat6 < 9)
    {
      lcd.setCursor(6, 1);
      lcd.print(nguongtat6, 0);
      lcd.print("     ");
    }
    else if (nguongtat6 < 99)
    {
      lcd.setCursor(6, 1);
      lcd.print(nguongtat6, 1);
      lcd.print("    ");
    }
    else if (nguongtat6 < 999)
    {
      lcd.setCursor(6, 1);
      lcd.print(nguongtat6);
      lcd.print("   ");
    }

    else if (nguongtat6 < 9999)
    {
      lcd.setCursor(6, 1);
      lcd.print(nguongtat6);
      lcd.print("  ");
    }
    else if (nguongtat6 < 99999)
    {
      lcd.setCursor(6, 1);
      lcd.print(nguongtat6);
      lcd.print(" ");
    }
    else
    {
      lcd.setCursor(6, 1);
      lcd.print(nguongtat6);
    }
    Blynk.virtualWrite(V28, nguongtat6);
    Blynk.virtualWrite(V27, nguongbat6);
  }

  if (demtong == 0)
  {
    readSensor();
    manhinhchinh();

    while (network.available())
    {
      RF24NetworkHeader header;

      network.read(header, &data, sizeof(data));

      // Serial debug: show header and payload
      Serial.print("NRF RX from node: ");
      Serial.println(header.from_node);
      Serial.print("Payload: ");
      size_t cnt = sizeof(data) / sizeof(data[0]);
      for (size_t i = 0; i < cnt; ++i)
      {
        Serial.print(data[i], 2);
        if (i + 1 < cnt)
          Serial.print(", ");
        else
          Serial.println();
      }

      if (header.from_node == 1)
      {
        data1 = data[0];
      }

      if (header.from_node == 2)
      {
        data2 = data[0];
        data1Valid = true;
        lastData1Update = millis();
        Serial.print("[RF2] data2=");
        Serial.print(data2, 2);
        Serial.print(" bat=");
        Serial.print(nguongbat1);
        Serial.print(" tat=");
        Serial.println(nguongtat1);
      }
      if (header.from_node == 3)
      {
        data3 = data[0];
      }

      if (header.from_node == 4)
      {
        data4 = data[0];
      }
    }

    if (WiFi.status() != WL_CONNECTED)
    {
      if (millis() - lastConnectionAttempt >= connectionDelay)
      {
        lastConnectionAttempt = millis();

        // attempt to connect to Wifi network:
        if (WiFi.psk().c_str() && strlen(WiFi.psk().c_str()))
        {
          WiFi.begin(WiFi.SSID().c_str(), WiFi.psk().c_str());
        }
        else
        {
          WiFi.begin(WiFi.SSID().c_str());
        }
      }
    }
    else
    {
      Blynk.run();
    }

    if (btchedo == 1)
    {
      // Đầu ra 1
      Blynk.virtualWrite(V29, 0);
      Blynk.virtualWrite(V30, 0);
      Blynk.virtualWrite(V31, 0);
      Blynk.virtualWrite(V32, 0);
      Blynk.virtualWrite(V33, 0);

      // trangthaibom1 = 0;
      // trangthaibom2 = 0;
      // trangthaibom3 = 0;
      // trangthaibom4 = 0;
      button6 = 0;

      if (trangthai == 1)
      {

        resetAllRelays();

        ledOut1.off();
        //            ledOut2.off();
        //            ledOut3.off();
        //            ledOut4.off();
        //            ledOut5.off();
        ledOut6.off();

        trangthai = 0;
      }
      if (data1Valid && (millis() - lastData1Update <= 5000))
      {
        driveHysteresisOutput(data2, nguongbat1, nguongtat1, RELAY1_PIN, ledOut1);
      }
      else
      {
        setOutputState(RELAY1_PIN, ledOut1, false);
      }
      driveHysteresisOutput(data2, nguongbat2, nguongtat2, RELAY2_PIN, ledOut2);
      driveHysteresisOutput(data3, nguongbat3, nguongtat3, RELAY3_PIN, ledOut3);
      driveHysteresisOutput(data4, nguongbat4, nguongtat4, RELAY4_PIN, ledOut4);
      driveHysteresisOutput(lux, nguongbat6, nguongtat6, RELAY6_PIN, ledOut6);
      //         if(data1 < nguongbat1 || data2 < nguongbat2 || data3 < nguongbat3 || data4 < nguongbat4){
      //          digitalWrite(RELAY5,HIGH);
      //          ledOut5.on();
      //          }
      //         else{
      //          digitalWrite(RELAY5,LOW);
      //          ledOut5.off();
      //          }
    }
    else if (btchedo == 0)
    {
      trangthai = 1;

      if (trangthaibom1 == 1)
      {
        setOutputState(RELAY1_PIN, ledOut1, true);
        // trangthai = 1;
      }
      else if (trangthaibom1 == 0)
      {
        setOutputState(RELAY1_PIN, ledOut1, false);
        // trangthai = 0;
      }
      // Đầu ra 2
      if (trangthaibom2 == 1)
      {
        setOutputState(RELAY2_PIN, ledOut2, true);
        // trangthai = 1;
      }
      else if (trangthaibom2 == 0)
      {
        setOutputState(RELAY2_PIN, ledOut2, false);
      }

      // Đầu ra 3
      if (trangthaibom3 == 1)
      {
        setOutputState(RELAY3_PIN, ledOut3, true);
        // trangthai = 1;
      }
      else if (trangthaibom3 == 0)
      {
        setOutputState(RELAY3_PIN, ledOut3, false);
        // trangthai = 0;
      }
      // Đầu ra 4
      if (trangthaibom4 == 1)
      {
        setOutputState(RELAY4_PIN, ledOut4, true);
        // trangthai = 1;
      }
      else if (trangthaibom4 == 0)
      {
        setOutputState(RELAY4_PIN, ledOut4, false);
        // trangthai = 0;
      }

      if (button6 == 1)
      {
        setOutputState(RELAY6_PIN, ledOut6, true);
        // trangthai = 1;
      }
      else if (button6 == 0)
      {
        setOutputState(RELAY6_PIN, ledOut6, false);
        // trangthai = 0;
      }
      if (button7 == 1)
      {
        setOutputState(RELAY5_PIN, ledOut5, true);
        // trangthai = 1;
      }
      else if (button7 == 0)
      {
        setOutputState(RELAY5_PIN, ledOut5, false);
        // trangthai = 0;
      }

      //         if(trangthaibom1 == 1 ||trangthaibom2 == 1 ||trangthaibom3 == 1 ||trangthaibom4 == 1){
      //          digitalWrite(RELAY5,HIGH);
      //          ledOut5.on();
      //          }
      //         else{
      //          digitalWrite(RELAY5,LOW);
      //          ledOut5.off();
      //          }
    }
  }
}
BLYNK_CONNECTED()
{
  Blynk.syncAll();
}

BLYNK_WRITE(V16)
{
  btchedo = param.asInt();
}

BLYNK_WRITE(V17)
{
  nguongbat1 = param.asInt();
  Serial.print("[THRESH1] bat=");
  Serial.println(nguongbat1);
}
BLYNK_WRITE(V18)
{
  nguongtat1 = param.asInt();
  Serial.print("[THRESH1] tat=");
  Serial.println(nguongtat1);
}
BLYNK_WRITE(V19)
{
  nguongbat2 = param.asInt();
}
BLYNK_WRITE(V20)
{
  nguongtat2 = param.asInt();
}
BLYNK_WRITE(V21)
{
  nguongbat3 = param.asInt();
}
BLYNK_WRITE(V22)
{
  nguongtat3 = param.asInt();
}
BLYNK_WRITE(V23)
{
  nguongbat4 = param.asInt();
}
BLYNK_WRITE(V24)
{
  nguongtat4 = param.asInt();
}
BLYNK_WRITE(V27)
{
  nguongbat6 = param.asInt();
}
BLYNK_WRITE(V28)
{
  nguongtat6 = param.asInt();
}
BLYNK_WRITE(V29)
{
  trangthaibom1 = param.asInt();
}
BLYNK_WRITE(V30)
{
  trangthaibom2 = param.asInt();
}
BLYNK_WRITE(V31)
{
  trangthaibom3 = param.asInt();
}
BLYNK_WRITE(V32)
{
  trangthaibom4 = param.asInt();
}
BLYNK_WRITE(V33)
{
  button6 = param.asInt();
}
BLYNK_WRITE(V34)
{
  button7 = param.asInt();
}
