/*
 Name:		SpeakerOnly.ino
 Created:	9/21/2020 9:03:04 PM
 Author:	YLA
*/

#include "NYG.h"
#include "Logger.h"
#include "IInput.h"
#include "IOutput.h"
#include "PushButton.h"
#include "Observer.h"
#include "Random.h"

enum
{
    BUTTON_PIN          = D5,
    SPEAKER_PIN         = D7,
};

PullupPushButton		button(BUTTON_PIN);
DigitalOutputPin		led(LED_BUILTIN);
ToneOutputPin			speaker(SPEAKER_PIN);
DigitalObserver			button_observer(button);

// the setup function runs once when you press reset or power the board
void setup() 
{
    Logger::Initialize();
    LOGGER << "Ready" << NL;
}

static unsigned long next = 0;

// the loop function runs over and over again until power down or reset
void loop() 
{
    unsigned long now = millis();

    bool pressed;
    bool button_changed = button_observer.TestChanged(pressed);

    static bool audible_sound = true;

    if (button_changed)
    {
        static unsigned long button_pressed = 0;

        if (pressed)
        {
            button_pressed = now;
        }
        else
        {
            long seconds = (now - button_pressed) / 1000;
            if (seconds >= 5)
            {
                audible_sound = !audible_sound;
                LOGGER << (audible_sound ? "HUMAN" : "DOG") << NL;
            }
            else
            {
                next = now - 1;
            }
        }
    }

    long delta = now - next;

    if (delta <= 0)
        return;

    enum { MIN_FREQUENCY    = 20000,
           MAX_FREQUENCY    = 30000,
           FREQUENCY_RANGE  = MAX_FREQUENCY - MIN_FREQUENCY,
           MAX_SOUND_DURATION_SECONDS   = 30,
           MAX_PAUSE_DURATION_SECONDS   = 15, };

    long seconds = ((Random::Get() % (speaker.Get() ? MAX_PAUSE_DURATION_SECONDS : MAX_SOUND_DURATION_SECONDS))) + 1;

    next = now + (seconds * 1000);

    if (speaker.Get())
    {
        LOGGER << "Quiet for " << seconds << " seconds" << NL;
        speaker.Quiet();
        led.Off();
        return;
    }

    // https://hobowithalaptop.com/stop-barking

    long frequency = MIN_FREQUENCY + (Random::Get() % FREQUENCY_RANGE);

    if (audible_sound)
        frequency /= 4;

    LOGGER << "Sound " << frequency << " Hz for " << seconds << " seconds" << NL;
    speaker.Tone((unsigned int)frequency);
    led.On();
}
