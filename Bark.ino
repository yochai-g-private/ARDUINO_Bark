/*
    Name:       Bark.ino
    Created:	3/9/2020 10:11:21 AM
    Author:     YOCHAIG-LPW10-2\MCP
*/

#include "NYG.h"
#include "MicroController.h"

using namespace NYG;

//TM_Inhibitor inhibitor;

void setup()
{
	Logger::Initialize();
	if (RTC::Begin())
	{
		LOGGER << "Current time: ";
		ShowTime();
	}
	else
	{
		LOGGER << "RTC failed to initialize" << NL;
	}

	LOGGER << "Ready!" << NL;
//	Serial.begin(115200);
}

void loop()
{
	if (Serial.available())
	{
		String s = Serial.readString();

		LOGGER << "Command '" << s << "' got from serial" << NL;

		if (RTC::SetFromSerial(s))
		{
			LOGGER << "New time: ";
			ShowTime();
			return;
		}

		if( s == "CT")
		{
			LOGGER << "Current time: ";
			ShowTime();
			return;
		}

		if (s == "RST")
		{
			LOGGER << "Restarting..." << NL;
			MicroController::Restart();
			return;
		}
	}

}

const char*	gbl_build_date = __DATE__;
const char*	gbl_build_time = __TIME__;
