#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WS2812FX.h>

/*
 * Key parameters for initializing program
 */
#define WIFI_SSID "kipi"
#define WIFI_PW "danik830"
#define WIFI_TIMEOUT 30000

#define LED_PIN 5
#define LED_COUNT 60

#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 5
#define DEFAULT_SPEED 2000

#define MODE_ACTIVE FX_MODE_CHASE_BLACKOUT
#define MODE_LETTER FX_MODE_STATIC

#define LETTERS 4

#define L1_START 46
#define L1_END 59

#define L2_START 31
#define L2_END 44

#define L3_START 16
#define L3_END 29

#define L4_START 0
#define L4_END 14

/*
 * Colours for each champion
 */

 #define LUX PINK
 #define ORI ORANGE
 #define NAMI CYAN
 #define SORAKA YELLOW
 #define MORG PURPLE
 #define SONA BLUE
 #define KARMA GREEN
 #define LULU MAGENTA
 #define RAKAN RED
 #define XERATH (uint32_t)0x007FFF
 #define VEIGAR (uint32_t)0x4B15CB
 #define CAIT (uint32_t)0xD2A52D
 #define ZYRA (uint32_t)0x71267E
 #define BRAND (uint32_t)0xCC300F
 #define BARD (uint32_t)0xF8D412
 #define AHRI (uint32_t)0xEFC3E9
 #define OTHER WHITE

/*
 * Various url extensions for making requests to riot api
 */
const String name = "Kipi";
const String summonerID = "summoner/v4/summoners/by-name/" + name;
const String matches = "match/v4/matchlists/by-account/";
const String inGame = "spectator/v4/active-games/by-summoner/";

/*
 * Function prototypes
 */
void wifi_setup();
void riot_api();
uint32_t set_color(String);

/*
 * Arrays for setting LED strip segments
 */
int letterStart[] = {L1_START, L2_START, L3_START, L4_START};
int letterEnd[] = {L1_END, L2_END, L3_END, L4_END};

/*
 * Necessary to illuminate LED strip
 */
WS2812FX led = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

/*
 * Variable for checking fetching new data every X seconds
 */
unsigned long wifi_check = 0;

/*
 * Various Riot API strings to reduce clutter in code 
 */
const String apiurl = "https://na1.api.riotgames.com/lol/";
const String ext = "?api_key=";
const String extIndex = "?endIndex=4&beginIndex=0&api_key=";
const String tKey = "your_key_here";
const String fingerprint = "your_fp_here";

/*
 * data is the data recovered from the riot api function
 * LEDCOLOR is the data recovered from the set_color function
 */
int count = 0;
String data[5];
uint32_t LEDCOLOR;

/*
 * Initial setup, runs once.
 * Sets initial colors for LED strip and connects to wi-fi.
 */
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Application Starts");

  Serial.println("LED Init");
  led.init();
  led.setSpeed(DEFAULT_SPEED);
  led.setMode(FX_MODE_CHASE_RAINBOW);
  led.setBrightness(DEFAULT_BRIGHTNESS);
  led.start();

  Serial.println("Wifi Setup");
  wifi_setup();

  Serial.println("Ready");
  }

/*
 * Looping function while the board has power. Basically runs the important functions every 30 seconds.
 */
void loop() {
  led.service();
  unsigned long now = millis();
  if (now - wifi_check > WIFI_TIMEOUT) {
    riot_api();
    if (data[0] == "no") {
    for (int i = 0; i < 5; i++) {
      LEDCOLOR = set_color(data[i + 1]);
      led.setSegment(i, letterStart[i], letterEnd[i], MODE_LETTER, LEDCOLOR, 2000, false);
    }
  }
    else if (data[0] == "yes") {
      led.setSegment(0, letterStart[0], letterEnd[0], FX_MODE_CHASE_RAINBOW, LEDCOLOR, 1000, false);
      for (int i = 1; i < 5; i++) {
        LEDCOLOR = set_color(data[i]);
        led.setSegment(i, letterStart[i], letterEnd[i], MODE_LETTER, LEDCOLOR, 2000, false);
      }
    }
    //disables LEDs to seperate segments
    led.setSegment(4, 15, 15, MODE_LETTER, BLACK, 2000, false);
    led.setSegment(5, 30, 30, MODE_LETTER, BLACK, 2000, false);
    led.setSegment(6, 45, 45, MODE_LETTER, BLACK, 2000, false);
    wifi_check = now;
  }
  }

/*
 * Function returns an led strip color depending on the champion id.
 */
uint32_t set_color(String id) {
  uint32_t color;
  if (id == "99") {
    return LUX;
  }
  else if (id == "61") {
    return ORI;
  }
  else if (id == "267") {
    return NAMI;
  }
  else if (id == "16") {
    return SORAKA;
  }
  else if (id == "25") {
    return MORG;
  }
  else if (id == "37") {
    return SONA;
  }
  else if (id == "43") {
    return KARMA;
  }
  else if (id == "117") {
    return LULU;
  }
  else if (id == "497") {
    return RAKAN;
  }
  else if (id == "101") {
    return XERATH;
  }
  else if (id == "45") {
    return VEIGAR;
  }
  else if (id == "51") {
    return CAIT;
  }
  else if (id == "143") {
    return ZYRA;
  }
  else if (id == "63") {
    return BRAND;
  }
  else if (id == "432") {
    return BARD;
  }
  else if (id == "103") {
    return AHRI;
  }
  else {
    return OTHER;
  }
}

/*
 * Function that connects to wi-fi. The program does not advance to loop() until a connection is initiated.
 */
void wifi_setup() {
	Serial.print("Connecting to ");
	Serial.println(WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PW);
	WiFi.mode(WIFI_STA);

	unsigned long connect_start = millis();

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");

		if (millis() - connect_start > WIFI_TIMEOUT) {
			Serial.println();
			Serial.println("Timeout, resetting...");
			ESP.reset();
		}
	}

	Serial.println();
	Serial.print("Connected to ");
	Serial.print(WiFi.localIP());
	Serial.println();
}

/*
 * The juice of the program, acquiring the last champs played and in game info is the end result.
 */
void riot_api() {
  HTTPClient http;
  String url = "";
  String payload = "";

  /*
   * First we have to find the summoner ID make additional requests
   */
  Serial.println("***Finding Summoner ID and Account ID***");
  url = apiurl + summonerID + ext + tKey;
  Serial.print("Fetching from: ");
  Serial.println(url);
  http.begin(url, fingerprint);
  int httpCode = http.GET();
  Serial.print("Response code: ");
  Serial.println(httpCode);

  String ID = "";
  String ingameID = "";
  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Text returned: ");
    Serial.println(payload);

    String summonerSS[7];
    int s = 0, j = 0;
    for (int i = 0; i < payload.length(); i++) {
    	if (payload.charAt(i) == ',') {
    		summonerSS[j] = payload.substring(s, i);
    		s = i + 1;
    		j++;
    	}
    }
    Serial.print("Summoner ID Substring: ");
    Serial.println(summonerSS[1]);
    String idSS[3];
    s = 1;
    j = 0;
    for (int i = 1; i < summonerSS[1].length(); i++) {
      if (summonerSS[1].charAt(i) == '"') {
        idSS[j] = summonerSS[1].substring(s, i);
        s = i + 1;
        j++;
      }
    }
    ID = idSS[2];
    s = 2;
    j = 0;
    for (int i = 2; i < summonerSS[0].length(); i++) {
      if (summonerSS[0].charAt(i) == '"') {
        idSS[j] = summonerSS[0].substring(s, i);
        s = i + 1;
        j++;
      }
    }
    //additionally here we get the account id, this is used to check if the summoner is currently in a game
    ingameID = idSS[2];
    Serial.print("Summoner ID = ");
    Serial.println(ID);
    Serial.print("Account ID = ");
    Serial.println(ingameID);
    
  }

  /*
   * Now we can look up past matches, we are only interested in the past 4 matches
   * We need to build a new url that uses the newly found summoner ID
   */
   Serial.println("***Finding Past 4 Matches***");
   url = apiurl + matches + ID + extIndex + tKey;
   Serial.print("Fetching from: ");
   Serial.println(url);
   http.begin(url, fingerprint);
   httpCode = http.GET();
   Serial.print("Response code: ");
   Serial.println(httpCode);

   if (httpCode > 0) {
    payload = http.getString();

    String matchSS[5];
    int s = 1, j = 0;
    for (int i = 1; i < payload.length(); i++) {
      if (payload.charAt(i) == '{') {
        matchSS[j] = payload.substring(s, i);
        s = i + 1;
        j++;
        if (j == 4) {
          matchSS[j] = payload.substring(s, payload.length() - 1);
          break;
        }
      }
    }
    s = 0;
    j = 0;
    String match1SS[8];
    for (int i = 1; i < matchSS[1].length(); i++) {
      if (matchSS[1].charAt(i) == ',') {
        match1SS[j] = matchSS[1].substring(s, i);
        s = i + 1;
        j++;
      }
    }
    match1SS[2].remove(0, 11);
    
    s = 0;
    j = 0;
    String match2SS[8];
    for (int i = 1; i < matchSS[2].length(); i++) {
      if (matchSS[2].charAt(i) == ',') {
        match2SS[j] = matchSS[2].substring(s, i);
        s = i + 1;
        j++;
      }
    }
    match2SS[2].remove(0, 11);
    
    s = 0;
    j = 0;
    String match3SS[8];
    for (int i = 1; i < matchSS[3].length(); i++) {
      if (matchSS[3].charAt(i) == ',') {
        match3SS[j] = matchSS[3].substring(s, i);
        s = i + 1;
        j++;
      }
    }

    match3SS[2].remove(0, 11);
    s = 0;
    j = 0;
    String match4SS[11];
    for (int i = 1; i < matchSS[4].length(); i++) {
      if (matchSS[4].charAt(i) == ',') {
        match4SS[j] = matchSS[4].substring(s, i);
        s = i + 1;
        j++;
      }
    }
    match4SS[2].remove(0, 11);
    
    String IG = "yes";
    Serial.println("***Checking if summoner is in a match***");
    url = apiurl + inGame + ingameID + ext + tKey;
    Serial.print("Fetching from: ");
    Serial.println(url);
    http.begin(url, fingerprint);
    httpCode = http.GET();
    Serial.print("Response code: ");
    Serial.println(httpCode);
    //if we get a 404 that means they are not in a match.
    if (httpCode == 404) {
      IG = "no";
    }
    http.end();

    /*
     * Finally after obtaining the data from the past four games, we put them in the global array that is procssed in the looping function.
     * Additionally, we check if they are also in a game.
     */
    data[0] = IG;
    data[1] = match1SS[2];
    data[2] = match2SS[2];
    data[3] = match3SS[2];
    data[4] = match4SS[2];
    Serial.println("Is summoner currently in a match? ");
    Serial.println(IG);
    Serial.println("4 most recent champions: ");
    Serial.println(match1SS[2]);
    Serial.println(match2SS[2]);
    Serial.println(match3SS[2]);
    Serial.println(match4SS[2]);
   }
  }
