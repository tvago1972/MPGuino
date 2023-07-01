#if defined(useButtonInput)
/* button input support section */

static const buttonVariable bpListMenu[] PROGMEM = {
	 {btnShortPressC,	menu::select}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::shortRight}
		,{btnShortPressU,	button::shortLeft}
#if defined(useLCDoutput)
		,{btnLongPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressL,	button::doNothing}
		,{btnShortPressR,	menu::select}
#else // useButtonCrossConfig
		,{btnShortPressR,	button::shortRight}
		,{btnShortPressL,	button::shortLeft}
#if defined(useLCDoutput)
		,{btnLongPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

static const buttonVariable bpListMainDisplay[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressR,	button::longRight}
	,{btnLongPressL,	button::longLeft}
#if defined(useScreenEditor)
	,{btnLongPressC,	displayEdit::entry}
	,{btnShortPressLR,	displayEdit::entry}
#endif // defined(useScreenEditor)
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::longRight}
#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useSavedTrips
		,{btnShortPressDR,	tripSupport::goSaveCurrent}
		,{btnShortPressDL,	tripSupport::goSaveTank}
	#endif // useSavedTrips
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR,	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#if defined(useScreenEditor)
static const buttonVariable bpListMainDisplayEdit[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressL,	displayEdit::cancel}
	,{btnLongPressLR,	displayEdit::cancel}
	,{btnLongPressC,	displayEdit::set}
	,{btnShortPressLR,	displayEdit::set}
	,{btnLongPressR,	displayEdit::readInitial}
#if defined(useButtonCrossConfig)
		,{btnShortPressU,	displayEdit::changeItemUp}
		,{btnShortPressD,	displayEdit::changeItemDown}
		,{btnShortPressC,	displayEdit::set}
#else // defined(useButtonCrossConfig)
		,{btnShortPressC,	displayEdit::changeItemUp}
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		button::noSupport}
};

#endif // defined(useScreenEditor)
static const buttonVariable bpListSetting[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressL,	menu::entry}
	,{btnShortPressLR,	parameterEdit::entry}
#ifdef useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
		,{btnShortPressD,	button::shortRight}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp, 		button::noSupport}
};

static const buttonVariable bpListParameterEdit[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressL,	parameterEdit::cancel}
	,{btnLongPressLR,	parameterEdit::cancel}
	,{btnLongPressC,	parameterEdit::save}
	,{btnShortPressLR,	parameterEdit::save}
	,{btnLongPressR,	parameterEdit::readInitial}
#ifdef useButtonCrossConfig
		,{btnShortPressUL,	parameterEdit::findLeft}
		,{btnShortPressUR,	parameterEdit::findRight}
		,{btnLongPressD,	parameterEdit::readMinValue}
		,{btnLongPressU,	parameterEdit::readMaxValue}
		,{btnShortPressU,	parameterEdit::changeDigitUp}
		,{btnShortPressD,	parameterEdit::changeDigitDown}
		,{btnShortPressC,	parameterEdit::save}
#else // useButtonCrossConfig
		,{btnShortPressLC,	parameterEdit::findLeft}
		,{btnShortPressCR,	parameterEdit::findRight}
		,{btnLongPressCR,	parameterEdit::readMinValue}
		,{btnLongPressLC,	parameterEdit::readMaxValue}
		,{btnShortPressC,	parameterEdit::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#if defined(useBigDigitDisplay) || defined(useStatusBar)
static const buttonVariable bpListBigNum[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressR,	button::longRight}
	,{btnLongPressL,	button::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::longRight}
#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // defined(useBigDigitDisplay) || defined(useStatusBar)
#ifdef useClockDisplay
static const buttonVariable bpListClockDisplay[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressR,	button::longRight}
	,{btnLongPressL,	button::longLeft}
	,{btnLongPressC,	clockSet::entry}
	,{btnShortPressLR,	clockSet::entry}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::longRight}
#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

static const buttonVariable bpListClockSet[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressL,	clockSet::cancel}
	,{btnLongPressLR,	clockSet::cancel}
	,{btnLongPressC,	clockSet::set}
	,{btnShortPressLR,	clockSet::set}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	clockSet::changeDigitUp}
		,{btnShortPressD,	clockSet::changeDigitDown}
		,{btnShortPressC,	clockSet::set}
#else // useButtonCrossConfig
		,{btnShortPressC,	clockSet::changeDigitUp}
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // useClockDisplay
#ifdef useCPUreading
static const buttonVariable bpListCPUmonitor[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressR,	button::longRight}
	,{btnLongPressL,	button::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::longRight}
#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
#else // useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // useCPUreading
#ifdef usePartialRefuel
static const buttonVariable bpListPartialRefuel[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressL,	mainDisplay::returnToMain}
	,{btnLongPressLR,	mainDisplay::returnToMain}
	,{btnLongPressC,	partialRefuel::longSelect}
	,{btnShortPressLR,	partialRefuel::select}
#ifdef useButtonCrossConfig
		,{btnShortPressU,	partialRefuel::select}
		,{btnLongPressU,	partialRefuel::longSelect}
		,{btnShortPressD,	button::shortRight}
		,{btnShortPressC,	mainDisplay::returnToMain}
#else // useButtonCrossConfig
		,{btnShortPressC,	partialRefuel::select}
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // usePartialRefuel
#if defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog)
static const buttonVariable bpListMiscViewer[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressL,	menu::entry}
#ifdef useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // defined(useSimulatedFIandVSS) || defined(useChryslerMAPCorrection) || defined(useDebugAnalog)
#ifdef useTestButtonValues
static const buttonVariable bpListButtonView[] PROGMEM = {
	 {buttonsUp,		button::doNothing}
};

#endif // useTestButtonValues
#ifdef useSavedTrips
static const buttonVariable bpListTripSave[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressL,	mainDisplay::returnToMain}
	,{btnLongPressLR,	mainDisplay::returnToMain}
	,{btnShortPressC,	mainDisplay::returnToMain}
#ifdef useButtonCrossConfig
		,{btnLongPressU,	tripSupport::select}
		,{btnShortPressD,	button::shortRight}
#else // useButtonCrossConfig
		,{btnLongPressC,	tripSupport::select}
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // useSavedTrips
#ifdef useBarGraph
static const buttonVariable bpListBarGraph[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressR,	button::longRight}
	,{btnLongPressL,	button::longLeft}
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::longRight}
#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressUL,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useBarFuelEconVsSpeed
		,{btnLongPressUC,	bgFEvsSsupport::reset}
	#endif // useBarFuelEconVsSpeed
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#ifdef usePartialRefuel
		,{btnLongPressLC,	partialRefuel::entry}
	#else // usePartialRefuel
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // usePartialRefuel
	#ifdef useBarFuelEconVsSpeed
		,{btnLongPressLCR,	bgFEvsSsupport::reset}
	#endif // useBarFuelEconVsSpeed
	#ifdef useCPUreading
		,{btnLongPressLR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // useBarGraph
#ifdef useButtonCrossConfig
#ifdef useDragRaceFunction
static const buttonVariable bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
	,{buttonsUp,		button::noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonVariable bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
	,{buttonsUp,		button::noSupport}
};

#endif // useCoastDownCalculator
#else // useButtonCrossConfig
#ifdef useDragRaceFunction
static const buttonVariable bpListDragRace[] PROGMEM = {
	{btnLongPressR,		accelerationTest::goTrigger}
	,{buttonsUp,		button::noSupport}
};

#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
static const buttonVariable bpListCoastdown[] PROGMEM = {
	{btnLongPressR,		coastdown::goTrigger}
	,{buttonsUp,		button::noSupport}
};

#endif // useCoastDownCalculator
#endif //useButtonCrossConfig
static const buttonVariablePointer menuButtonList[(uint16_t)(displayCountTotal)] PROGMEM = {

// the following screen entries are in the top-down menu list

	 bpListMainDisplay
#if defined(useStatusBar)
	,bpListBigNum
#endif // defined(useStatusBar)
#ifdef useBigFE
	,bpListBigNum
#endif // useBigFE
#ifdef useBarFuelEconVsTime
	,bpListBarGraph
#endif // useBarFuelEconVsTime
#ifdef useBarFuelEconVsSpeed
	,bpListBarGraph
#endif // useBarFuelEconVsSpeed
#ifdef useBigDTE
	,bpListBigNum
#endif // useBigDTE
#ifdef useBigTTE
	,bpListBigNum
#endif // useBigTTE
#ifdef useCPUreading
	,bpListCPUmonitor
#endif // useCPUreading
#ifdef useClockDisplay
	,bpListClockDisplay
#endif // useClockDisplay
	,bpListSetting
	,bpListSetting
	,bpListSetting
	,bpListSetting
#ifdef useChryslerMAPCorrection
	,bpListSetting
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,bpListSetting
#endif // defined(useCoastDownCalculator) or defined(useDragRaceFunction)
	,bpListSetting
	,bpListSetting
#ifdef useDragRaceFunction
	,bpListDragRace
#endif // useDragRaceFunction
#ifdef useCoastDownCalculator
	,bpListCoastdown
#endif // useCoastDownCalculator
#ifdef useSimulatedFIandVSS
	,bpListMiscViewer
#endif // useSimulatedFIandVSS
#ifdef useChryslerMAPCorrection
	,bpListMiscViewer
#endif // useChryslerMAPCorrection
#ifdef useDebugAnalog
	,bpListMiscViewer
#endif // useDebugAnalog
#ifdef useTestButtonValues
	,bpListButtonView
#endif // useTestButtonValues

// the following screen entries do not show up in the top-down menu list

	,bpListMenu
	,bpListParameterEdit
#ifdef useClockDisplay
	,bpListClockSet
#endif // useClockDisplay
#ifdef useSavedTrips
	,bpListTripSave
#endif // useSavedTrips
#ifdef usePartialRefuel
	,bpListPartialRefuel
#endif // usePartialRefuel
#if defined(useScreenEditor)
	,bpListMainDisplayEdit
#endif // defined(useScreenEditor)
};

static void button::init(void)
{

#if defined(useLegacyButtons)
	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega2560__)
	DIDR2 &= ~((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // enable digital input on port K button pins

	PORTK |= ((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // enable port K button pullup resistors

	PCMSK2 |= ((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // enable port K button interrupts

	lastPINxState = PINK; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DIDR0 &= ~((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // enable digital input on port C button pins

	PORTC |= ((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // enable port C button pullup resistors

	PCMSK1 |= ((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // enable port C button interrupts

	lastPINxState = PINC; // initialize last input pin state value so as to not erroneously detect a button press on start

#endif // defined(__AVR_ATmega328P__)
	SREG = oldSREG; // restore interrupt flag status

#endif // defined(useLegacyButtons)
#if defined(useTWIbuttons)
#if defined(useInterruptBasedTWI)
	TWI::disableISRactivity(); // disable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)
#if defined(useAdafruitRGBLCDshield)
	MCP23017portExpanderSupport::configureOutputPort((uint16_t)(buttonMask));
#endif // defined(useAdafruitRGBLCDshield)
#if defined(useInterruptBasedTWI)
	TWI::enableISRactivity(); // enable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)

#endif // defined(useTWIbuttons)
#if defined(useAnalogButtons)
	changeBitFlags(timer0Command, 0, t0cEnableAnalogButtons); // enable analog button sampling

#endif // defined(useAnalogButtons)
}

static void button::shutdown(void)
{

#if defined(useLegacyButtons)
	uint8_t oldSREG;

#endif // defined(useLegacyButtons)
#if defined(useAnalogButtons)
	changeBitFlags(timer0Command, t0cEnableAnalogButtons, 0); // disable analog button sampling

#endif // defined(useAnalogButtons)
#if defined(useTWIbuttons)
#if defined(useInterruptBasedTWI)
	TWI::disableISRactivity(); // disable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)
#if defined(useAdafruitRGBLCDshield)
	MCP23017portExpanderSupport::configureOutputPort(0);
#endif // defined(useAdafruitRGBLCDshield)
#if defined(useInterruptBasedTWI)
	TWI::enableISRactivity(); // enable ISR-based TWI activity
#endif // defined(useInterruptBasedTWI)

#endif // defined(useTWIbuttons)
#if defined(useLegacyButtons)
	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts

#if defined(__AVR_ATmega2560__)
	DIDR2 |= ((1 << ADC13D) | (1 << ADC12D) | (1 << ADC11D)); // disable digital input on port K button pins

	PORTK &= ~((1 << PORTK5) | (1 << PORTK4) | (1 << PORTK3)); // disable port K button pullup resistors

	PCMSK2 &= ~((1 << PCINT21) | (1 << PCINT20) | (1 << PCINT19)); // disable port K button interrupts

#endif // defined(__AVR_ATmega2560__)
#if defined(__AVR_ATmega328P__)
	DIDR0 |= ((1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D)); // disable digital input on port C button pins

	PORTC &= ~((1 << PORTC5) | (1 << PORTC4) | (1 << PORTC3)); // disable port C button pullup resistors

	PCMSK1 &= ~((1 << PCINT13) | (1 << PCINT12) | (1 << PCINT11)); // disable port C button interrupts

#endif // defined(__AVR_ATmega328P__)
	SREG = oldSREG; // restore interrupt flag status

#endif // defined(useLegacyButtons)
}

static void button::doCommand(void)
{

	const buttonVariable * bpPtr;
	uint8_t bp;
	uint8_t i;

	bp = buttonPress; // capture button state
	bpPtr = (const buttonVariable *)(pgm_read_word(&(menuButtonList[(unsigned int)(menuLevel)].buttonVariableList)));

	while (true)
	{

		i = pgm_read_byte(&(bpPtr->buttonCode));

		if ((bp == i) || (i == buttonsUp)) break;
		else bpPtr++;

	}

	text::gotoXY(devLCD, 0, 0);
	((bdFunc)pgm_read_word(&(bpPtr->buttonCommand)))(); // go perform action

}

static void button::noSupport(void)
{

	text::initStatus(devLCD);
	text::stringOut(devLCD, PSTR("Btn 0x"));
	text::hexByteOut(devLCD, buttonPress);
	text::stringOut(devLCD, PSTR(" Pressed"));
	delayS(holdDelay);

}

static void button::doNothing(void)
{
}

#if defined(useAnalogButtons) || defined(useDebugButtonInjection)
static void button::inject(uint8_t buttonValue)
{

	uint8_t oldSREG;

	oldSREG = SREG; // save interrupt flag status
	cli(); // disable interrupts to make the next operations atomic

	thisButtonState = buttonValue;
	timer0Command |= (t0cProcessButton); // send timer0 notification that a button was just read in

	SREG = oldSREG; // restore interrupt flag status

}

#endif // defined(useAnalogButtons) || defined(useDebugButtonInjection)
#if defined(useLCDoutput)
static void button::doNextBright(void)
{

	text::initStatus(devLCD);
	text::stringOut(devLCD, PSTR("Backlight = "));
	text::stringOut(devLCD, brightString, brightnessIdx);
	delayS(holdDelay);

}

#endif // defined(useLCDoutput)
static void button::shortLeft(void)
{

	cursor::moveRelative(0, 255); // go to previous option in current level

}

static void button::shortRight(void)
{

	cursor::moveRelative(0, 1); // go to next option in current level

}

static void button::longLeft(void)
{

	cursor::moveRelative(255, 0); // go to last option in previous level

}

static void button::longRight(void)
{

	cursor::moveRelative(1, 0); // go to next option in next level

}

#endif // defined(useButtonInput)
