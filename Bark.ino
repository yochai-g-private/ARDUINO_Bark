/*
    Name:       Bark.ino
    Created:	3/9/2020 10:11:21 AM
    Author:     YOCHAIG-LPW10-2\MCP
*/

#include "NYG.h"
#include "IInput.h"
#include "IOutput.h"
#include "PushButton.h"
#include "Observer.h"
#include "Hysteresis.h"
#include "DipSwitch.h"
#include "SevenSegmentDisplay.h"

enum
{
    DEEP_SWITCH_1_PIN               = D2,
    DEEP_SWITCH_2_PIN               = D3,
    DEEP_SWITCH_3_PIN               = D4,

	SPEAKER_PIN			            = D7,

    DISPLAY_CLK_PIN                 = D9,
    DISPLAY_DIO_PIN                 = D10,

	PITCH_POTENTIOMETER_PIN	        = A7,
    SENSITIVITY_POTENTIOMETER_PIN   = A6,
    LED_PIN				            = A4,
	BUTTON_PIN			            = A1,
    NOISE_SENSOR_PIN                = A0,
};

PullupPushButton		button(BUTTON_PIN);
DigitalOutputPin		led(LED_PIN);
ToneOutputPin			speaker(SPEAKER_PIN);
AnalogInputPin			pitch_potentiometer(PITCH_POTENTIOMETER_PIN);
AnalogInputPin          sensitivity_potentiometer(SENSITIVITY_POTENTIOMETER_PIN);

DigitalObserver			button_observer(button);

DivisorHysteresis		pitch(pitch_potentiometer, 100),
                        sensitivity(sensitivity_potentiometer, 100);
AnalogObserver			pitch_observer(pitch),
            			sensitivity_observer(sensitivity);

SevenSegmentDisplay     display(DISPLAY_CLK_PIN, DISPLAY_DIO_PIN);

Pin                     ds_pins[3] = { DEEP_SWITCH_1_PIN, DEEP_SWITCH_2_PIN, DEEP_SWITCH_3_PIN };
DipSwitch<3>            ds(ds_pins);

static void check_pitch_changed();
static void check_sensitivity_changed();
static bool is_barking();
static bool check_toggle_repellering();
static bool is_repellering();
static void start_repeller();

static int              st_NOISE_THRESHOLD;
static int              st_analog_NOISE_THRESHOLD;
static unsigned long    st_repellering_started;
static uint8_t          st_repellering_seconds;
static int              st_analog_BASE_FREQ;
static unsigned int     st_BASE_FREQ;
//---------------------------------------------------------------------
void setup()
{
	Logger::Initialize();

    check_sensitivity_changed();    delay(1000);
    check_pitch_changed();          delay(1000);

    display.PrintText(" OK ");
    LOGGER << "Ready!" << NL;
}
//---------------------------------------------------------------------
void loop()
{
    check_pitch_changed();

    if (check_toggle_repellering())
        return;

    if (is_repellering())
        return;

    check_sensitivity_changed();

    if (is_barking())
        start_repeller();
}
//---------------------------------------------------------------------
static bool is_barking()
{
    unsigned int noise_level = 0;

    #define USE_MAX4466_MICROPHONE_AMPLIFIER    1

#if USE_MAX4466_MICROPHONE_AMPLIFIER

    #define SAMPLE_WINDOW 50

    unsigned long startMillis = millis();  // Start of sample window
    ;   // peak-to-peak level

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    // collect data for SAMPLE_WINDOW mS
    while (millis() - startMillis < SAMPLE_WINDOW)
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

    noise_level = (signalMax >= signalMin) ? signalMax - signalMin : 0;
#else
    noise_level = analogRead(0);// NOISE_SENSOR_PIN);
#endif

    bool noise = noise_level > st_NOISE_THRESHOLD;

    if(noise)
    {
        LOGGER << "Noise level: " << noise_level << NL;
        //delay(200);
    }

    return noise;
}
//---------------------------------------------------------------------
static void start_repeller()
{
    st_repellering_started = millis();

#if 1
    st_repellering_seconds = ds.Get();
#else
    st_repellering_seconds = ds_1.Get() ? 1 : 0 +
                             ds_2.Get() ? 2 : 0 +
                             ds_3.Get() ? 4 : 0;
#endif

    st_repellering_seconds *= 10;

    if (!st_repellering_seconds)
        st_repellering_seconds = 2;
                             
    LOGGER << "Repeller started for " << st_repellering_seconds  << " seconds" << NL;
    led.On();
}
//---------------------------------------------------------------------
static void stop_repeller()
{
    st_repellering_started = 0;
    speaker.Quiet();

    LOGGER << "Repeller stopped" << NL;
    led.Off();

    delay(500);
}
//---------------------------------------------------------------------
static bool is_repellering()
{
    if (!st_repellering_started)
        return false;
    
    if ((millis() - st_repellering_started) / 1000 >= st_repellering_seconds)
    {
        stop_repeller();
        return false;
    }

    static int degrees = 0;
    
    degrees++;
    degrees = degrees % 360;

    float sinVal = sin(degrees * (PI / 180));
    
    #define SIN_FACTOR	(st_BASE_FREQ / 4)

    unsigned int toneVal = st_BASE_FREQ + sinVal * SIN_FACTOR;

    //LOGGER << "Pitch = " << toneVal << NL;

    speaker.Tone(toneVal);

    return true;
}
//---------------------------------------------------------------------
static void toggle_repellering()
{
    if (st_repellering_started)
        stop_repeller();
    else
        start_repeller();
}
//---------------------------------------------------------------------
static void display_F()
{
    char text[16];
    sprintf(text, "F %2d", st_analog_BASE_FREQ);
    display.PrintText(text, false);
}
//---------------------------------------------------------------------
static unsigned int unscale(unsigned int scaled_value, unsigned int scale_divisor, unsigned int min, unsigned int max)
{
    return min + ((max - min) / (1024 / scale_divisor)) * scaled_value;
}
//---------------------------------------------------------------------
static void check_pitch_changed()
{
    int analog;
    
    bool changed = pitch_observer.TestChanged(analog);

    if (changed)
    {
        st_analog_BASE_FREQ = analog;

        enum { MIN_FREQ = 1000, MAX_FREQ = 35000 };

        st_BASE_FREQ = unscale(analog, pitch.GetDeviation(), MIN_FREQ, MAX_FREQ);
//        st_BASE_FREQ = MIN_FREQ + ((MAX_FREQ - MIN_FREQ) / (1024 / pitch.GetDeviation())) * analog;

        LOGGER << "Analog pitch=" << analog << ", Sound frequency=" << st_BASE_FREQ << NL;

        display_F();
    }
}
//---------------------------------------------------------------------
static void display_S()
{
    char text[16];
    sprintf(text, "S %2d", st_analog_NOISE_THRESHOLD);
    display.PrintText(text, false);
}
//---------------------------------------------------------------------
static void check_sensitivity_changed()
{
    int analog;

    bool changed = sensitivity_observer.TestChanged(analog);

    if (changed)
    {
        int reverted_analog = (1024 / sensitivity.GetDeviation()) - analog;

        st_analog_NOISE_THRESHOLD = analog;

        enum { MIN_SENS = 600, MAX_SENS = 1000 };

        st_NOISE_THRESHOLD = unscale(reverted_analog, sensitivity.GetDeviation(), MIN_SENS, MAX_SENS);
        //st_NOISE_THRESHOLD = reverted_analog * sensitivity.GetDeviation();
        
        LOGGER << "Scaled sensitivity=" << analog << ", Analog sensitivity=" << st_NOISE_THRESHOLD << NL;

        display_S();
    }
}
//---------------------------------------------------------------------
static bool check_toggle_repellering()
{
    bool pressed;
    bool button_changed = button_observer.TestChanged(pressed);

    if (button_changed)
    {
        LOGGER << "Button " << (pressed ? "pressed" : "released") << NL;

        if (pressed)
        {
            toggle_repellering();
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------
