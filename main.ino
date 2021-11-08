//  _ ___ _______     ___ ___ ___  ___ _   _ ___ _____ ___
// / |_  )__ /   \   / __|_ _| _ \/ __| | | |_ _|_   _/ __|
// | |/ / |_ \ |) | | (__ | ||   / (__| |_| || |  | | \__ \ 
// |_/___|___/___/   \___|___|_|_\\___|\___/|___| |_| |___/
//
// Control of Eletronic Bell
// School: Stalim Romano
// Made by Felipe Schieber e Thiago Soares
// Orientator: Djim Martins
// License: CC-BY-SA 3.0

// Arduino Uno/2009:
// ----------------------
// DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin

#include <LiquidCrystal.h>
#include <DS3231.h>
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// Init the DS3231 using the hardware interface
DS3231 rtc(SDA, SCL);

// Init a Time-data structure
Time t;
Time hora_aula;

int testButton = 5;
int ledPin = 13;
int pin_menu_button = 6;
int pin_botao_ajuste = 7;
int buttonState = 0;       // the current reading from the input pin
int lastButtonState = LOW; // the previous reading from the input pin

int buttonStatePlus;           // the current reading from the input pin
int lastButtonStatePlus = LOW; // the previous reading from the input pin

int buttonStateMinus;           // the current reading from the input pin
int lastButtonStateMinus = LOW; // the previous reading from the input pin

// the following variables are unsigned long's because the time, measured in miliseconds,
// will quickly become a bigger number than can't be stored in an int.
unsigned long lastDebounceTime = 0;      // the last time the output pin was toggled
unsigned long debounceDelay = 2000;      // the debounce time; increase if the output flickers (2 segundos) ok
unsigned long debounceDelayAjuste = 200; // the debounce time; increase if the output flickers (200 milisegundos) ok

int segundo, minuto, hora, dia, mes, ano;
int pin_sino_rele = 3; // digital pin to toggle relay. Used to turn the bell on and off.

int duracao_aula_matutina_vespertina = 50;      // in minutes
int duracao_aula_noturna = 45;                  // in minutes
int duracao_intervalo_matutino_vespertino = 20; // in minutes
int duracao_intervalo_nortuno = 15;             // in minutes

int primeira_hora_aula_matutina = 7;
int hora_intervalo_matutino = 9;
int ultima_hora_aula_matutina = 11;

int primeira_hora_aula_vespertina = 13;
int hora_intervalo_vespertino = 15;
int ultima_hora_aula_vespertina = 17;

int primeira_hora_aula_noturna = 19;
int hora_intervalo_noturno = 21;
int ultima_hora_aula_noturna = 22;

void tocaSino()
{
    /*lcd.clear();   
  lcd.setCursor(0,0);   
  lcd.print("Tocando Sino...");   
  Serial.println("Tocando Sino..."); */

    digitalWrite(pin_sino_rele, LOW);  // ring the bell
    delay(4000);                       // leave the bell ringing for 4 seconds
    digitalWrite(pin_sino_rele, HIGH); // turn off the bell
}

boolean adjust_btn_minus()
{
    // read the state of the switch into a local variable:
    int reading = digitalRead(pin_menu_button);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonStateMinus)
    {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelayAjuste)
    {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonStateMinus)
        {
            buttonStateMinus = reading;

            // only toggle the LED if the new button state is HIGH
            if (buttonStateMinus == HIGH)
            {
                return true;
                //rtc.setTime(11, 14, 0);     // Set the time to 12:00:00 (24hr format)
            }
        }
    }

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonStateMinus = reading;
}

boolean adjust_btn_plus()
{
    // read the state of the switch into a local variable:
    int reading = digitalRead(pin_botao_ajuste);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonStatePlus)
    {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelayAjuste)
    {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonStatePlus)
        {
            buttonStatePlus = reading;

            // only toggle the LED if the new button state is HIGH
            if (buttonStatePlus == HIGH)
            {
                return true;
                //rtc.setTime(11, 14, 0);     // Set the time to 12:00:00 (24hr format)
            }
        }
    }

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonStatePlus = reading;
}

boolean menu_button()
{
    // read the state of the switch into a local variable:
    int reading = digitalRead(pin_menu_button);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH),  and you've waited
    // long enough since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState)
    {
        // reset the debouncing timer
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
        // whatever the reading is at, it's been there for longer
        // than the debounce delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != buttonState)
        {
            buttonState = reading;

            // only toggle the LED if the new button state is HIGH
            if (buttonState == HIGH)
            {
                return true;
                //rtc.setTime(11, 14, 0);     // Set the time to 12:00:00 (24hr format)
            }
        }
    }

    // save the reading.  Next time through the loop,
    // it'll be the lastButtonState:
    lastButtonState = reading;
}

void ajuste_hora_certa()
{
    boolean hora_nova = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ajuste de Hora");

    lcd.setCursor(0, 1);
    lcd.print(t.hour);

    while (!hora_nova)
    {
        if (adjust_btn_minus())
        {
            if (hora == 0)
            {
                hora = 24;
            }
            hora = hora - 1;
            rtc.setTime(hora, 0, 0);
            hora_nova = true;
        }
        if (adjust_btn_plus())
        {
            if (hora == 24)
            {
                hora = 0;
            }
            hora = hora + 1;
            rtc.setTime(hora, 0, 0);
            hora_nova = true;
        }
    }
}

void setup()
{
    rtc.begin();

    // The following lines can be uncommented to set the date and time
    //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
    //rtc.setTime(13, 37, 0);     // Set the time to 12:00:00 (24hr format)
    //rtc.setDate(6, 8, 2017);   // Set the date to January 1st, 2014

    pinMode(pin_sino_rele, OUTPUT); //Indica que o pin do sino é de saída.
    pinMode(pin_menu_button, INPUT);
    pinMode(pin_botao_ajuste, INPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(testButton, INPUT);

    digitalWrite(pin_sino_rele, HIGH);

    Serial.begin(115200);
    lcd.begin(16, 2);

    lcd.setCursor(0, 0);
    lcd.print("      Sinal");
    lcd.setCursor(0, 1);
    lcd.print(" Stalim Romano");
    delay(2000);
    lcd.clear();
}

void loop()
{

    // read the state of the pushbutton value:
    buttonState = digitalRead(testButton);

    // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
    if (buttonState == HIGH)
    {
        // turn LED on:
        digitalWrite(ledPin, HIGH);
        Serial.print("Botão Apertado");
    }
    else
    {
        // turn LED off:
        digitalWrite(ledPin, LOW);
    }
    t = rtc.getTime();
    lcd.setCursor(0, 0);
    lcd.print("Data: ");
    lcd.print(rtc.getDateStr());
    Serial.print("  ");
    Serial.print(rtc.getDateStr());

    lcd.setCursor(0, 1);
    lcd.print("Hora: ");
    lcd.print(rtc.getTimeStr());

    Serial.print("-- Hora: ");
    Serial.println(rtc.getTimeStr());

    int hour = t.hour;
    int min = t.min;
    int sec = t.sec;
    char time[] = hour + ":" + min + ":" + sec;

    char *times[20] = [ "7:0:0", "7:50:0", "8:40:0", "9:30:0", "9:50:0", "10:40:0", "11:30:0", "13:0:0", "13:50:0", "14:40:0", "15:30:0", "15:50:0", "16:40:0", "17:30:0", "19:30:0", "19:45:0", "20:30:0", "21:15:0", "21:30:0", "22:15:0" ];

    for (int i = 0; i < 20; i++)
    {
        if (time == times[i])
        {
            tocaSino();
            break;
        }
        if (atoi(times[i][0]) > hour)
        {
            break;
        }
    }
    delay(1000);
}
