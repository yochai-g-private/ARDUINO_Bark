/*
 Name:		TestNoiseSensor.ino
 Created:	3/10/2020 5:00:44 PM
 Author:	MCP
*/

const int ledPin = 13; //pin 13 built-in led
const int soundPin = A0; //sound sensor attach to A0


// the setup function runs once when you press reset or power the board
void setup() {
	pinMode(ledPin, OUTPUT);//set pin13 as OUTPUT
	Serial.begin(115200); //initialize serial
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	int value = analogRead(soundPin);//read the value of A0
	//Serial.println(value);//print the value
	if (value > 600) //if the value is greater than 600
	{
        Serial.println(value);
		digitalWrite(ledPin, HIGH);//turn on the led
		delay(200);//delay 200ms
	}
	else
	{
		digitalWrite(ledPin, LOW);//turn off the led
	}

    //delay(100);
}
