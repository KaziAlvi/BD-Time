#include <WiFi.h>
#include <time.h>
#include <U8x8lib.h>

//const char* ssid = "Access point's SSID";              
//const char* password = "Access point's PASSWORD";  
const char* ssid = "System1";
const char* password = "Kfg654321!@#";    

const char* NTP_SERVER = "ch.pool.ntp.org";
const char* TZ_INFO    = "UTC -6";  // enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)

tm timeinfo;
time_t now;
long unsigned lastNTPtime;
unsigned long lastEntryTime;

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

void setup() 
{
  u8x8.begin();
  
  Serial.begin(115200);
  Serial.println("\n\nNTP Time Test\n");
  WiFi.begin(ssid, password);

  Serial.print("Connecting to network");
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);    
    if (++counter > 100) 
      ESP.restart();
    Serial.print( "." );
  }
  Serial.println("\nWiFi connected\n\n");

  configTime(0, 0, NTP_SERVER);
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
  setenv("TZ", TZ_INFO, 1);

  if (getNTPtime(10)) 
  {  
    // wait up to 10sec to sync
  } 
  else 
  {
    Serial.println("Time not set");
    ESP.restart();
  }
  showTime(&timeinfo);
  lastNTPtime = time(&now);
  lastEntryTime = millis();
}

void loop() 
{
  getNTPtime(10);
  showTime(&timeinfo);
  delay(1000);
}

bool getNTPtime(int sec) 
{
  {
    uint32_t start = millis();
    do
    {
      time(&now);
      localtime_r(&now, &timeinfo);
      delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    
    if (timeinfo.tm_year <= (2016 - 1900)) 
        return false;  // the NTP call was not successful
    
    Serial.print("Time Now: ");  
    Serial.println(now); 
  }
  return true;
}

void showTime(tm *localTime) 
{
  //print to serial terminal
  Serial.print(localTime->tm_mday);
  Serial.print('/');
  Serial.print(localTime->tm_mon + 1);
  Serial.print('/');
  Serial.print(localTime->tm_year - 100);
  Serial.print('-');
  Serial.print(localTime->tm_hour);
  Serial.print(':');
  Serial.print(localTime->tm_min);
  Serial.print(':');
  Serial.print(localTime->tm_sec);
  Serial.print(" Day of Week ");
  Serial.println(localTime->tm_wday);
  Serial.println();

  //display on OLED
  char time_output[30];
  
  u8x8.setFont(u8x8_font_courB18_2x3_f);
  u8x8.setCursor(0,0);
  sprintf(time_output, "%02d:%02d:%02d", localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
  u8x8.print(time_output);
  
  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(4,4);
  sprintf(time_output, "%02d/%02d/%02d", localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year - 100);
  u8x8.print(time_output);
  
  u8x8.setCursor(4,6);
  u8x8.print(getDOW(localTime->tm_wday));
}

char * getDOW(uint8_t tm_wday)
{
  switch(tm_wday)
  {
    case 1:
      return "Monday";
      break;

    case 2:
      return "Tuesday";
      break;

    case 3:
      return "Wednesday";
      break;

    case 4:
      return "Thursday";
      break;

    case 5:
      return "Friday";
      break;

    case 6:
      return "Saturday";
      break;

    case 7:
      return "Sunday";
      break;

    default:
      return "Error";
      break;
  }
}
