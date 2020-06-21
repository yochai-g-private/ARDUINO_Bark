/*
 Name:		TestBuzzer.ino
 Created:	3/9/2020 10:16:07 AM
 Author:	MCP
*/

int buzzerPin = 4;    // the number of the buzzer pin
float sinVal;         // Define a variable to save sine value
int toneVal;          // Define a variable to save sound frequency
bool on;


// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(buzzerPin, OUTPUT); // Set Buzzer pin to output mode
	tone(buzzerPin, 440);
}

// the loop function runs over and over again until power down or reset
void loop() {
	//on = !on;
	//digitalWrite(4, on ? HIGH : LOW);
	//delay(5000);
	return;
	#define BASE_FREQ	5000
	#define SIN_FACTOR	(BASE_FREQ / 4)

	for (int x = 0; x < 360; x++) {						// X from 0 degree->360 degree
		sinVal = sin(x * (PI / 180));					// Calculate the sine of x
		toneVal = BASE_FREQ + sinVal * SIN_FACTOR;      // Calculate sound frequency according to the sine of x
		tone(buzzerPin, toneVal);						// Output sound frequency to buzzerPin
		delay(10);
	}
}
