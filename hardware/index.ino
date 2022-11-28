#include <string.h>
#include <LiquidCrystal.h>
#define pin_trig 12
#define pin_echo 13

unsigned char S[256];
char has[512];
#define S_SWAP(a,b) do { int t = S[a]; S[a] = S[b]; S[b] = t; } while(0)

const char *host = "https://hooks.zapier.com";
const int port = 80;
const char *baseUri = "/hooks/catch/13889573/bn6u1tk/";

const int errorPin = 13;
int n = 0;

const char *ssid = "Simulator Wifi";
const char *password = "";
const int rs = 2, en = 3, d0 = 4, d1 = 5, d2 = 6, d3 = 7, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d0, d1, d2, d3, d4, d5, d6, d7);
const int lcd_cols = 16;
const int lcd_rows = 2;

double tempo_leitura;
int delta_time;
int t_resposta = 500;
long metade_Vsom = 0.01715;

int esp8266(String comm, int miliseconds, char *str)
{
    int e = 0;
    Serial.println(comm);
    delay(miliseconds);
    if (!Serial.find(str))
    {
        digitalWrite(errorPin, HIGH);
        e = 1;
    }
    return e;
}

void wait_time()
{
    String msg = "";
    char buffer[10];
    lcd.noBlink();
    sprintf(buffer, "%03d", n);
    msg.concat(buffer);
    lcd.setCursor(lcd_cols - 3, lcd_rows - 1);
    lcd.print(buffer);
    lcd.setCursor(lcd_cols - 1, lcd_rows - 1);
    lcd.blink();
}

void lcd_scroll(String msg)
{
    lcd.setCursor(0, 0);
    lcd.print(msg);
    delay(150);
    for (int p = 0; p < 24; p++)
    {
        lcd.scrollDisplayLeft();
        delay(150);
    }
    for (int p = 0; p < 24; p++)
    {
        lcd.scrollDisplayRight();
        delay(150);
    }
}

void lcd_print(String msg, int r = 0)
{
    String msg2;
    int wc_x, wc_y;
    int l = msg.length();
    lcd.setCursor(0, r);
    if (l > lcd_cols)
    {
        if (r >= lcd_rows - 1)
        {
            msg2 = msg.substring(0, lcd_cols - 3) + "...";
            lcd.print(msg2);
            wc_x = msg2.length() - 1;
            wc_y = r;
        }
        else
        {
            lcd.print(msg.substring(0, lcd_cols));
            wc_x = lcd_cols - 1;
            wc_y = r + 1;
            lcd_print(msg.substring(lcd_cols), r + 1);
        }
    }
    else
    {
        lcd.print(msg);
        wc_x = msg.length() - 1;
        wc_y = r;
    }
}

void rc4(char *key, char *data)
{
    int i, j;

    Serial.print("Fonte : ");
    Serial.println(data);
    Serial.print("Chave : ");
    Serial.println(key);

    for (i = 0; i < 256; i++)
    {
        S[i] = i;
    }

    j = 0;
    for (i = 0; i < 256; i++)
    {
        j = (j + S[i] + key[i % strlen(key)]) % 256;
        S_SWAP(S[i], S[j]);
    }

    i = j = 0;
    for (int k = 0; k < strlen(data); k++)
    {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        S_SWAP(S[i], S[j]);
        has[k] = data[k] ^ S[(S[i] + S[j]) % 256];
    }
    has[strlen(data) + 1] = '\0';
}

void setup(void)
{
    Serial.begin(115200);
    lcd.begin(lcd_cols, lcd_rows);
    pinMode(errorPin, OUTPUT);
    delay(100);
    esp8266("AT", 10, "OK");
    esp8266("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"", 10, "OK");
    esp8266("AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(port), 50, "OK");
    pinMode(pin_trig, OUTPUT);
    pinMode(pin_echo, INPUT);
    digitalWrite(pin_trig, LOW);

    char key[] = "55621516";
    char sdata[] = "Testando criptografia";
    rc4(key, sdata);
    Serial.print("Criptografado : ");
    Serial.println(has);
    rc4(key, has);
    Serial.print("Descriptografado : ");
    Serial.println(has);
}

void display_error(String msg, int nn)
{
    lcd_print(msg);
    digitalWrite(errorPin, HIGH);
    n = nn;
}

void send_request(bool isParked)
{
    String isParkedQueryParam = isParked ? "true" : "false";
    String uri = String(baseUri) + "?hardwareId=1&" + "isParked=" + isParkedQueryParam;

  	String httpPacket = "GET " + String(uri) +
                    " HTTP/1.1\r\n" +
                    "Host: " + String(host) + "\r\n" +
                    "Connection: close\r\n\r\n";

    esp8266("AT+CIPSEND=" + String(httpPacket.length()), 100, ">");
    int e = esp8266(httpPacket, 10, "SEND OK\r\n");
    if (e != 0)
        display_error("Comm failed (1)!", n);
}

void read_stream()
{
    unsigned long timeout = millis();
    while (Serial.available() == 0)
    {
        if (millis() - timeout > 5000)
        {
            display_error("Comm failed (2)!", 10);
            return;
        }
    }
    if (Serial.find("\r\n\r\n"))
    {
        delay(5);
        unsigned int i = 0;
        String message = "";
        while (i < 60000)
        {
            if (Serial.available())
            {
                message += (char)Serial.read();
                i = 0;
            }
            i++;
        }
        lcd_scroll(message);
    }
    else
    {
        display_error("Comm failed (3)!", 15);
    }
}

long microsecondsToCentimeters(long microseconds)
{
    return microseconds / 29 / 2;
}

void loop(void)
{
    if (millis() - tempo_leitura < t_resposta)
    {
        tempo_leitura = millis();
    }

    digitalWrite(pin_trig, HIGH);
    delayMicroseconds(11);
    digitalWrite(pin_trig, LOW);

    long delta_time = pulseIn(pin_echo, HIGH);

    long dist = microsecondsToCentimeters(delta_time);

    wait_time();
    if (n <= 0)
    {
        n = 1000;
        send_request(dist < 200);
        read_stream();
    }
    else
    {
        delay(1000);
    }
    n--;
}
