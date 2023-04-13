#ifdef useCoastDownCalculator
namespace coastdown /* Coastdown Calculator support section prototype */
{

	void goDisplay(void);
	void goTrigger(void);

}

volatile uint8_t coastdownFlags;
volatile uint8_t coastdownState;

const uint8_t cdtActive =				0b10000000;
const uint8_t cdtTriggered =			0b01000000;
const uint8_t cdtCancelled =			0b00100000;
const uint8_t cdtFinished =				0b00010000;
const uint8_t cdSignalStateChange =		0b00001000;
const uint8_t cdTakeSample =			0b00000100;
const uint8_t cdSampleTaken =			0b00000010;

const uint8_t cdtTestInProgress =		cdtActive | cdtTriggered;
const uint8_t cdtTestClearFlags =		cdtTestInProgress | cdtCancelled | cdtFinished | cdSignalStateChange | cdTakeSample | cdSampleTaken;

#endif // useCoastDownCalculator
