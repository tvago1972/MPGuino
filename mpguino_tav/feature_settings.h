const uint8_t nesLoadInitial =			0;
const uint8_t nesLoadValue =			nesLoadInitial + 1;
const uint8_t nesOnChange =				nesLoadValue + 1;
const uint8_t nesSaveParameter =		nesOnChange + 1;
const uint8_t nesOutputUpperScreen =	nesSaveParameter + 1;

typedef struct
{

	uint8_t callingScreenLevel; // remember what module called number editor
	uint8_t parameterIdx;
	const char * neStatusMessage;

} numberEditType;

static numberEditType numberEditObj;

namespace parameterEdit /* parameter editor/entry section prototype */
{

	static uint8_t sharedFunctionCall(uint8_t cmd);
	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void findLeft(void);
	static void findRight(void);
	static void readInitial(void);
	static void readMinValue(void);
	static void readMaxValue(void);
	static void changeDigitUp(void);
	static void changeDigitDown(void);
	static void changeDigit(uint8_t dir);
	static void save(void);
	static void cancel(void);
	static uint8_t onEEPROMchange(const uint8_t * sched, uint8_t parameterIdx);

};

namespace settings /* Parameter setting selector section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);

};

#ifdef usePartialRefuel
namespace partialRefuel /* partial refuelling support section prototype */
{

	static uint8_t displayHandler(uint8_t cmd, uint8_t cursorPos, uint8_t cursorChanged);
	static void entry(void);
	static void select(void);
	static void longSelect(void);

}

const char partialRefuelFuncNames[] PROGMEM = {
	"Add Partial\r"
	"Zero Partial\r"
	"Reset Tank Trip\r"
};

static const char prStatusMessages[] PROGMEM = {
	"No change\r"
	"Added Partial\r"
	"Canx Partial Add\r"
};

#endif // usePartialRefuel
static const char numberEditSave[] PROGMEM = {
	" OK"
};

static const char numberEditCancel[] PROGMEM = {
	" XX"
};
