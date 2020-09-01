/*
 Name:		TestUltrasonicSpeaker.ino
 Created:	5/10/2020 10:39:26 PM
 Author:	YLA
*/

#include "NYG.h"
#include "IInput.h"
#include "IOutput.h"
#include "PushButton.h"
#include "Observer.h"
#include "Hysteresis.h"

using namespace NYG;

enum
{
	BUTTON_PIN			= D3,
	SPEAKER_PIN			= D7,
	POTENTIOMETER_PIN	= A7,
	LED_PIN				= A4,
    NOISE_SENSOR_PIN    = A0,
};

PullupPushButton		button(BUTTON_PIN);
DigitalOutputPin		led(LED_PIN);
ToneOutputPin			speaker(SPEAKER_PIN);
AnalogInputPin			potentiometer(POTENTIOMETER_PIN);

DigitalObserver			button_observer(button);
DivisorHysteresis		pitch(potentiometer, 100);
AnalogObserver			pitch_observer(pitch);

unsigned int sound_frequency;

// the setup function runs once when you press reset or power the board
void setup() 
{
	Logger::Initialize();

	LOGGER << "Ready" << NL;
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	int sound_pitch;
	bool pitch_changed = pitch_observer.TestChanged(sound_pitch);
	
	if (pitch_changed)
	{
		enum { MIN_FREQ = 1000, MAX_FREQ = 30000 };

		sound_frequency = MIN_FREQ + ((MAX_FREQ - MIN_FREQ) / (1024 / pitch.GetDeviation())) * sound_pitch;

		LOGGER << "Pitch=" << sound_pitch << ", Frequency=" << sound_frequency << NL;
	}

	bool pressed;
	bool button_changed = button_observer.TestChanged(pressed);

	if (button_changed)
	{
		LOGGER << "Button " << (pressed ? "pressed" : "released") << NL;
		//led.Set(pressed);

		if (pressed)
			speaker.Tone(sound_frequency);
		else
			speaker.Quiet();
	}
	else
	{
		if(pitch_changed && speaker.Get())
			speaker.Tone(sound_frequency);
	}
#if 1
    #define sampleWindow 50

    unsigned long startMillis = millis();  // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    // collect data for 50 mS
    while (millis() - startMillis < sampleWindow)
    {
        unsigned int sample = analogRead(0);
        if (sample < 1024)  // toss out spurious readings
        {
            if (sample > signalMax)
            {
                signalMax = sample;  // save just the max levels
            }
            else if (sample < signalMin)
            {
                signalMin = sample;  // save just the min levels
            }
        }
    }
    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
    //double volts = (peakToPeak * 5.0) / 1024;  // convert to volts

    led.Set(peakToPeak > 500 ? HIGH : LOW);
#else
    int value = analogRead(NOISE_SENSOR_PIN);//read the value of A0
    //Serial.println(value);//print the value
    if (value > 600) //if the value is greater than 600
    {
        led.On();// digitalWrite(ledPin, HIGH);//turn on the led
        //delay(200);//delay 200ms
    }
    else
    {
        led.Off(); //digitalWrite(ledPin, LOW);//turn off the led
    }
#endif
	//delay(100);
}
