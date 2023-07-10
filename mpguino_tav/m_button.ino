#if defined(useButtonInput)
/* button input support section */

static const buttonVariable bpListMenu[] PROGMEM = {
	 {btnShortPressC,	menu::select}
	,{btnLongPressL,	menu::exit}
#if defined(useLCDoutput)
	,{btnLongPressC,	button::doNextBright}
#endif // defined(useLCDoutput)
#if defined(useButtonCrossConfig)
		,{btnShortPressU,	button::shortLeft}
		,{btnShortPressD,	button::shortRight}
		,{btnShortPressL,	menu::exit}
		,{btnShortPressR,	menu::select}
#else // defined(useButtonCrossConfig)
		,{btnShortPressL,	button::shortLeft}
		,{btnShortPressR,	button::shortRight}
#endif // defined(useButtonCrossConfig)
	,{buttonsUp,		button::noSupport}
};

static const buttonVariable bpListMainDisplay[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressR,	button::longRight}
	,{btnLongPressL,	button::longLeft}
#if LCDcharHeight == 4
	,{btnLongPressC,	transferDisplay}
#endif // LCDcharHeight == 4
#if defined(useScreenEditor)
	,{btnShortPressLR,	displayEdit::entry}
#endif // defined(useScreenEditor)
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::longRight}
	#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
	#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
	#if defined(useEnhancedTripReset)
		,{btnShortPressUL,	tripSave::goSaveTank}
		,{btnLongPressUL,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressUR,	tripSave::goSaveCurrent}
		,{btnLongPressUR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
	#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
	#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
	#if defined(useEnhancedTripReset)
		,{btnShortPressLC,	tripSave::goSaveTank}
		,{btnLongPressLC,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressCR,	tripSave::goSaveCurrent}
		,{btnLongPressCR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
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

#if defined(useBigDigitDisplay) || defined(useStatusBar) || defined(useCPUreading) || defined(useBarGraph)
static const buttonVariable blListSecondaryDisplay[] PROGMEM = {
	 {btnShortPressR,	button::shortRight}
	,{btnShortPressL,	button::shortLeft}
	,{btnLongPressR,	button::longRight}
	,{btnLongPressL,	button::longLeft}
#if LCDcharHeight == 4
	,{btnLongPressC,	transferDisplay}
#endif // LCDcharHeight == 4
#ifdef useButtonCrossConfig
		,{btnShortPressD,	button::longRight}
	#if defined(useLCDoutput)
		,{btnShortPressU,	button::doNextBright}
	#endif // defined(useLCDoutput)
		,{btnShortPressC,	menu::entry}
	#if defined(useEnhancedTripReset)
		,{btnShortPressUL,	tripSave::goSaveTank}
		,{btnLongPressUL,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressUR,	tripSave::goSaveCurrent}
		,{btnLongPressUR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
	#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
	#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
	#if defined(useEnhancedTripReset)
		,{btnShortPressLC,	tripSave::goSaveTank}
		,{btnLongPressLC,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressCR,	tripSave::goSaveCurrent}
		,{btnLongPressCR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#ifdef useCPUreading
		,{btnLongPressLCR, 	systemInfo::showCPUloading}
	#endif // useCPUreading
#endif // useButtonCrossConfig
	,{buttonsUp,		button::noSupport}
};

#endif // defined(useBigDigitDisplay) || defined(useStatusBar) || defined(useCPUreading) || defined(useBarGraph)
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
	#if defined(useEnhancedTripReset)
		,{btnShortPressUL,	tripSave::goSaveTank}
		,{btnLongPressUL,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressUL,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressUR,	tripSave::goSaveCurrent}
		,{btnLongPressUR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressUR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
	#ifdef useCPUreading
		,{btnLongPressU,	systemInfo::showCPUloading}
	#endif // useCPUreading
#else // useButtonCrossConfig
	#if defined(useLCDoutput)
		,{btnShortPressC,	button::doNextBright}
	#endif // defined(useLCDoutput)
		,{btnShortPressLCR,	menu::entry}
	#if defined(useEnhancedTripReset)
		,{btnShortPressLC,	tripSave::goSaveTank}
		,{btnLongPressLC,	tripSave::goSaveTank}
	#else // defined(useEnhancedTripReset)
		,{btnLongPressLC,	tripSupport::resetTank}
	#endif // defined(useEnhancedTripReset)
	#if defined(useSavedTrips)
		,{btnShortPressCR,	tripSave::goSaveCurrent}
		,{btnLongPressCR,	tripSave::goSaveCurrent}
	#else // defined(useSavedTrips)
		,{btnLongPressCR,	tripSupport::resetCurrent}
	#endif // defined(useSavedTrips)
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
	,blListSecondaryDisplay
#endif // defined(useStatusBar)
#if defined(useBigFE)
	,blListSecondaryDisplay
#endif // defined(useBigFE)
#if defined(useBarFuelEconVsTime)
	,blListSecondaryDisplay
#endif // defined(useBarFuelEconVsTime)
#if defined(useBarFuelEconVsSpeed)
	,blListSecondaryDisplay
#endif // defined(useBarFuelEconVsSpeed)
#if defined(useBigDTE)
	,blListSecondaryDisplay
#endif // defined(useBigDTE)
#if defined(useBigTTE)
	,blListSecondaryDisplay
#endif // defined(useBigTTE)
#if defined(useCPUreading)
	,blListSecondaryDisplay
#endif // defined(useCPUreading)
#ifdef useClockDisplay
	,bpListClockDisplay
#endif // useClockDisplay
	,bpListMenu
	,bpListMenu
	,bpListMenu
	,bpListMenu
#ifdef useChryslerMAPCorrection
	,bpListMenu
#endif // useChryslerMAPCorrection
#if defined(useCoastDownCalculator) || defined(useDragRaceFunction)
	,bpListMenu
#endif // defined(useCoastDownCalculator) || defined(useDragRaceFunction)
	,bpListMenu
	,bpListMenu
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
#if defined(useExpandedMainDisplay)
	,bpListMenu
#endif // defined(useExpandedMainDisplay)
	,bpListMenu
	,bpListParameterEdit
#ifdef useClockDisplay
	,bpListClockSet
#endif // useClockDisplay
#if defined(useSavedTrips)
	,bpListMenu
#endif // defined(useSavedTrips)
#if defined(useEnhancedTripReset)
	,bpListMenu
#endif // defined(useEnhancedTripReset)
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
	bpPtr = (const buttonVariable *)(pgm_read_word(&(menuButtonList[(unsigned int)(thisMenuData.displayIdx)].buttonVariableList)));

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
