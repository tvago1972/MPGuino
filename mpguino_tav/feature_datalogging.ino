#if defined(useDataLoggingOutput)
static void doOutputDataLog(void)
{

	uint8_t c = ',';

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		// perform the required decimal formatting, and output the number
		text::tripFunctionOut(m8DevLogOutputIdx, pgm_read_word(&dataLogTripCalcFormats[(uint16_t)(x)]), 0, (dfOverflow9s));
		text::charOut(m8DevLogOutputIdx, c);

	}

}

#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
/* JSON formatting support section */

static void JSONsupport::init(uint8_t devIdx)
{

	JSONlevel = 0;
	JSONflag[(uint16_t)(JSONlevel)] = 0;

	text::stringOut(devIdx, PSTR("\n\n")); // begin JSON payload

}

static void JSONsupport::openElement(uint8_t devIdx, uint8_t elementType)
{

	outputElementNext(devIdx, elementType);

	JSONflag[(uint16_t)(JSONlevel)] |= (elementType);

	JSONlevel++;
	JSONflag[(uint16_t)(JSONlevel)] = 0;

	outputElementStart(devIdx, elementType);

}

static void JSONsupport::openKey(uint8_t devIdx, const char * str, uint8_t elementType)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) closeElementInternal(devIdx);

	openElement(devIdx, 0);

	text::charOut(devIdx, '"');
	text::stringOut(devIdx, str);
	text::stringOut(devIdx, PSTR("\":"));

	JSONflag[(uint16_t)(JSONlevel)] |= (elementType | JSONflagKey);

	outputElementStart(devIdx, elementType);

}

static void JSONsupport::closeElement(uint8_t devIdx)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) closeElementInternal(devIdx);

	closeElementInternal(devIdx);

}

static void JSONsupport::closeElementInternal(uint8_t devIdx)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) outputElementEnd(devIdx);

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagString) text::charOut(devIdx, '"');

	JSONlevel--;

	outputElementEnd(devIdx);

	if (JSONlevel == 0) text::stringOut(devIdx, PSTR("\n\n")); // end JSON payload, and go trigger read on python.script

}

static void JSONsupport::outputElementStart(uint8_t devIdx, uint8_t elementType)
{

	if (elementType & JSONflagArray) text::charOut(devIdx, '[');

	if (elementType & JSONflagObject) text::charOut(devIdx, '{');

	if (elementType & JSONflagString) text::charOut(devIdx, '"');

}

static void JSONsupport::outputElementNext(uint8_t devIdx, uint8_t elementType)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagString)
	{

		text::charOut(devIdx, '"');
		JSONflag[(uint16_t)(JSONlevel)] &= ~(JSONflagString);

	}

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagMany) text::charOut(devIdx, ',');

	JSONflag[(uint16_t)(JSONlevel)] |= (JSONflagMany);

	if (elementType & JSONflagString)
	{

		text::charOut(devIdx, '"');
		JSONflag[(uint16_t)(JSONlevel)] |= (JSONflagString);

	}

}

static void JSONsupport::outputElementEnd(uint8_t devIdx)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagArray) text::charOut(devIdx, ']');

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagObject) text::charOut(devIdx, '}');

}

static void JSONsupport::outputNumber(uint8_t devIdx, uint8_t tripIdx, uint8_t calcIdx)
{

	outputElementNext(devIdx, 0);
	text::tripFunctionOut(devIdx, tripIdx, calcIdx, 0, (dfOverflow9s));

}

static void JSONsupport::outputNumber(uint8_t devIdx, const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces)
{

	outputElementNext(devIdx, 0);
	SWEET64::runPrgm(sched, tripIdx);
	text::stringOut(devIdx, ull2str(nBuff, decimalPlaces, 0, (dfOverflow9s))); // output the number part

}

static void JSONsupport::outputNumber(uint8_t devIdx, uint32_t an, uint8_t decimalPlaces)
{

	outputElementNext(devIdx, 0);
	SWEET64::init64((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), an);
	text::stringOut(devIdx, ull2str(nBuff, decimalPlaces, 0, (dfOverflow9s))); // output the number part

}

/* skybolt JSON packet output support section */

static const uint8_t prgmFindHalfReserveRange[] PROGMEM = {
	instrLdRegVariable, 0x02, m64TankSizeIdx,			// fetch calculated tank size in injector open cycles
	instrSubVariableFromX, 0x02, m64BingoTankSizeIdx,	// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrShiftRegRight, 0x02,							// shift result right one bit
	instrJump, tCalculateFuelDistance					// go format it
};

#if defined(useDragRaceFunction)
static void doOutputJSONnumber(uint32_t an, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::init64((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), an);
	text::stringOut(m8DevJSONoutputIdx, ull2str(nBuff, decimalPlaces, 0, (dfOverflow9s))); // output the number part
	text::stringOut(m8DevJSONoutputIdx, labelStr); // output the label part

}

#endif // defined(useDragRaceFunction)
static void doOutputJSONfuelEcon(const char * prefixStr, uint8_t tripIdx)
{

	translateCalcIdx(tripIdx, tFuelEcon, 4, (dfOverflow9s));

	text::stringOut(m8DevJSONoutputIdx, prefixStr);

	if (mainCalcFuncVar.value > 9999999)
	{

		text::stringOut(m8DevJSONoutputIdx, PSTR("infinite "));
		text::stringOut(m8DevJSONoutputIdx, mainCalcFuncVar.calcFormatLabelPtr);

	}
	else text::numberOut(m8DevJSONoutputIdx, (dfOutputLabel));

}

static void doOutputJSONremainingFuel(void)
{

	translateCalcIdx(0, tReserveRemainingFuel, 4, (dfOverflow9s)); // reserve remaining fuel left

	if (mainCalcFuncVar.value == 0) text::stringOut(m8DevJSONoutputIdx, PSTR("fumes")); // fumes left
	else text::numberOut(m8DevJSONoutputIdx, (dfOutputLabel)); // reserve remaining fuel left

	text::stringOut(m8DevJSONoutputIdx, PSTR(" remaining, "));
	text::tripFunctionOut(m8DevJSONoutputIdx, 0, tBingoRemainingFuel, 0, (dfOverflow9s | dfOutputLabel)); // bingo remaining fuel left
	text::stringOut(m8DevJSONoutputIdx, PSTR(" e-reserve"));

}

static void doOutputJSON(void) //skybolt added JSON output function
{

	uint8_t oldSREG;
	static uint8_t subtitleCount1 = 2;
#if defined(useDragRaceFunction)
	static uint8_t subtitleCount2 = 3;

	uint32_t targetSpeed;
	uint32_t targetDistance;
#endif // defined(useDragRaceFunction)

	// replaced timerChecker with this because it's a more accurate method to change once every 1.6 seconds
	if (volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] & t0saJSONchangeSubtitle)
	{

		oldSREG = SREG; // save interrupt flag status
		cli(); // disable interrupts to make the next operation atomic

		volatile8Variables[(uint16_t)(v8Timer0Status0Idx - v8VariableStartIdx)] &= ~(t0saJSONchangeSubtitle);

		SREG = oldSREG; // restore interrupt flag status

		if (!(--subtitleCount1)) subtitleCount1 = 2;
#if defined(useDragRaceFunction)
		if (!(--subtitleCount2)) subtitleCount2 = 3;
#endif // defined(useDragRaceFunction)

	}

	JSONsupport::init(m8DevJSONoutputIdx); // begin JSON payload

	JSONsupport::openElement(m8DevJSONoutputIdx, JSONflagArray);

	// first graph, fuel

	JSONsupport::openElement(m8DevJSONoutputIdx, JSONflagObject);

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONtitleStr, JSONflagString);
	doOutputJSONremainingFuel();

	// text::tripFunctionOut(m8DevJSONoutputIdx, instantIdx, tEngineSpeed, 0, (dfOverflow9s | dfOutputLabel)); // rpm to test latency only vs tachometer and LCD vs raspi indicator (expect 2x looptime)

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			text::stringOut(m8DevJSONoutputIdx, PSTR("fuel used: "));
			text::stringOut(m8DevJSONoutputIdx, tripStr);
			text::tripFunctionOut(m8DevJSONoutputIdx, currentIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // current trip fuel used
			text::stringOut(m8DevJSONoutputIdx, semicolonTankStr);
			text::tripFunctionOut(m8DevJSONoutputIdx, tankIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // tank trip fuel used
			text::stringOut(m8DevJSONoutputIdx, semicolonUsingStr);
			text::tripFunctionOut(m8DevJSONoutputIdx, instantIdx, tFuelRate, 0, (dfOverflow9s | dfOutputLabel)); // current rate of fuel burn in units/time

			break;

		case 1:
			text::stringOut(m8DevJSONoutputIdx, PSTR("eco stats: "));
#if defined(trackIdleEOCdata)
			text::stringOut(m8DevJSONoutputIdx, PSTR("used@idle "));
			text::tripFunctionOut(m8DevJSONoutputIdx, eocIdleTankIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel));
			text::stringOut(m8DevJSONoutputIdx, PSTR(", fuel cut "));
			text::tripFunctionOut(m8DevJSONoutputIdx, eocIdleTankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel));
#else // defined(trackIdleEOCdata)
			doOutputJSONremainingFuel();
#endif // defined(trackIdleEOCdata)

			break;

	}

	//ranges do not have to be in order, d3js libraries will auto sort, so you can make it easier to read the code by changing the order
	JSONsupport::openKey(m8DevJSONoutputIdx, JSONrangesStr, JSONflagArray);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tFuelQuantity); // largest, full tank size (e.g, 13.8 g)
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tReserveQuantity); // full tank less reserve (e.g.13.8g - 2.2g = 11.6g)
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tBingoQuantity); // reserve amount (e.g. 2.2g)

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tReserveRemainingFuel); // reserve remaining fuel left
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tRemainingFuel); // total remaining fuel left

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONmarkersStr, JSONflagArray);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, instantIdx, tFuelRate); // current rate of fuel burn in units/time

	JSONsupport::closeElement(m8DevJSONoutputIdx);

	// second graph, distance
	JSONsupport::openElement(m8DevJSONoutputIdx, JSONflagObject);

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONtitleStr, JSONflagString);
	text::tripFunctionOut(m8DevJSONoutputIdx, tankIdx, tReserveDistanceToEmpty, 0, (dfOverflow9s | dfOutputLabel)); // distance to bingo
	text::stringOut(m8DevJSONoutputIdx, PSTR(" to e, "));
	text::tripFunctionOut(m8DevJSONoutputIdx, tankIdx, tBingoDistanceToEmpty, 0, (dfOverflow9s | dfOutputLabel)); // distance to fully empty tank from bingo
	text::stringOut(m8DevJSONoutputIdx, PSTR(" e-reserve"));

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			//	text::stringOut(m8DevJSONoutputIdx, PSTR("trip/tank distance: "));
			//	text::tripFunctionOut(m8DevJSONoutputIdx, currentIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			//	text::stringOut(m8DevJSONoutputIdx, PSTR("/"));
			//	text::tripFunctionOut(m8DevJSONoutputIdx, tankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance

			text::tripFunctionOut(m8DevJSONoutputIdx, currentIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			text::stringOut(m8DevJSONoutputIdx, PSTR(" trip distance, "));
			text::tripFunctionOut(m8DevJSONoutputIdx, tankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			text::stringOut(m8DevJSONoutputIdx, PSTR(" tank distance"));
			break;

		case 1:
			text::tripFunctionOut(m8DevJSONoutputIdx, tankIdx, tReserveDistance, 0, (dfOverflow9s | dfOutputLabel)); // reserve range
			text::stringOut(m8DevJSONoutputIdx, PSTR(" safe range, "));
			text::tripFunctionOut(m8DevJSONoutputIdx, tankIdx, tRangeDistance, 0, (dfOverflow9s | dfOutputLabel)); // distance to fully empty tank
			text::stringOut(m8DevJSONoutputIdx, PSTR(" dry range"));
			break;

	}

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONrangesStr, JSONflagArray);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tRangeDistance); // maximum range
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tReserveDistance); // range 2, safe range
	JSONsupport::outputNumber(m8DevJSONoutputIdx, prgmFindHalfReserveRange, tankIdx, 1); // range 3, half of safe range

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tBingoDistance); // shows miles of e-reserve in bar form

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONmarkersStr, JSONflagArray);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tDistanceToEmpty); // line is distance to empty

	JSONsupport::closeElement(m8DevJSONoutputIdx);

	// third graph, econ

	JSONsupport::openElement(m8DevJSONoutputIdx, JSONflagObject);

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONtitleStr, JSONflagString);
#if defined(useDragRaceFunction)

	if (accelTestState) //display if we have encountered a state change in the drag test
	{

		text::stringOut(m8DevJSONoutputIdx, findStr(JSONaccelTestStateMsgs, accelTestState));
		text::stringOut(m8DevJSONoutputIdx, PSTR(" ..."));

	}
	else // else not racing or waiting, go to normal
	{

		doOutputJSONfuelEcon(PSTR("fuel economy trip/tank/inst: "), currentIdx); // gal or L, then current fuel economy
		doOutputJSONfuelEcon(PSTR("/ "), tankIdx); // tank fuel economy
		doOutputJSONfuelEcon(PSTR("/ "), instantIdx); // instantaneous fuel economy

	}
#else // defined(useDragRaceFunction)
	doOutputJSONfuelEcon(PSTR("fuel economy trip/tank/inst: "), currentIdx); // gal or L, then current fuel economy
	doOutputJSONfuelEcon(PSTR("/ "), tankIdx); // tank fuel economy
	doOutputJSONfuelEcon(PSTR("/ "), instantIdx); // instantaneous fuel economy
#endif // defined(useDragRaceFunction)

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONsubtitleStr, JSONflagString);
#if defined(useDragRaceFunction)
	targetSpeed = SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx); // accel test speed
	targetDistance = SWEET64::runPrgm(prgmFetchParameterValue, pDragDistanceIdx); // accel test distance

	switch (subtitleCount2)
	{

		case 3:
			//1 & 2 seconds display
			text::stringOut(m8DevJSONoutputIdx, PSTR("accel time: 0-"));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR("/"));
			doOutputJSONnumber(targetSpeed, 0, PSTR(": "));

			text::tripFunctionOut(m8DevJSONoutputIdx, dragHalfSpeedIdx, tAccelTestTime, 0, dfOverflow9s); // 0-(half speed) time
			text::stringOut(m8DevJSONoutputIdx, PSTR("/"));
			text::tripFunctionOut(m8DevJSONoutputIdx, dragFullSpeedIdx, tAccelTestTime, 0, dfOverflow9s); // 0-(full speed) time
			text::stringOut(m8DevJSONoutputIdx, PSTR(", "));
			text::tripFunctionOut(m8DevJSONoutputIdx, dragDistanceIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // trap distance
			text::stringOut(m8DevJSONoutputIdx, PSTR(" in "));
			text::tripFunctionOut(m8DevJSONoutputIdx, dragDistanceIdx, tAccelTestTime, 0, (dfOverflow9s | dfOutputLabel)); // trap time
			text::stringOut(m8DevJSONoutputIdx, PSTR(" @"));
			text::tripFunctionOut(m8DevJSONoutputIdx, dragDistanceIdx, tTrapSpeed, 0, (dfOverflow9s | dfOutputLabel)); // trap speed
			text::stringOut(m8DevJSONoutputIdx, PSTR("; "));
			text::tripFunctionOut(m8DevJSONoutputIdx, dragDistanceIdx, tEstimatedEnginePower, 0, (dfOverflow9s | dfOutputLabel)); // estimated engine power
			text::stringOut(m8DevJSONoutputIdx, PSTR(" @"));
			text::tripFunctionOut(m8DevJSONoutputIdx, dragDistanceIdx, tDragSpeed, 0, (dfOverflow9s | dfOutputLabel)); // max speed
			break;

		case 2:
			text::stringOut(m8DevJSONoutputIdx, PSTR("accel fuel: "));
			text::tripFunctionOut(m8DevJSONoutputIdx, dragHalfSpeedIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // 0-(half speed) fuel
			text::stringOut(m8DevJSONoutputIdx, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			text::tripFunctionOut(m8DevJSONoutputIdx, dragFullSpeedIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // 0-(full speed) fuel
			text::stringOut(m8DevJSONoutputIdx, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			text::tripFunctionOut(m8DevJSONoutputIdx, dragDistanceIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // trap fuel
			text::stringOut(m8DevJSONoutputIdx, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

		case 1:
			text::stringOut(m8DevJSONoutputIdx, PSTR("accel fuel: "));

			text::tripFunctionOut(m8DevJSONoutputIdx, dragHalfSpeedIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // 0-(half speed) mpg
			text::stringOut(m8DevJSONoutputIdx, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			text::tripFunctionOut(m8DevJSONoutputIdx, dragFullSpeedIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // 0-(full speed) mpg
			text::stringOut(m8DevJSONoutputIdx, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			text::tripFunctionOut(m8DevJSONoutputIdx, dragDistanceIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // trap mpg
			text::stringOut(m8DevJSONoutputIdx, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

	}
#else // defined(useDragRaceFunction)
	text::stringOut(m8DevJSONoutputIdx, PSTR("[this space intentionally left blank]"));
#endif // defined(useDragRaceFunction)

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONrangesStr, JSONflagArray);
	// set scale at 40mpg or instant econ up to 999 mpg. Folks like to watch their mpg meter go to extremes
	JSONsupport::outputNumber(m8DevJSONoutputIdx, 18000ul, 3);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, 24000ul, 3);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, min(max(40000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 999000), 3);

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(m8DevJSONoutputIdx, currentIdx, tFuelEcon); // current fuel economy
	JSONsupport::outputNumber(m8DevJSONoutputIdx, tankIdx, tFuelEcon); // tank fuel economy

	JSONsupport::openKey(m8DevJSONoutputIdx, JSONmarkersStr, JSONflagArray);
	// instantaneous fuel economy, do not let scale exceed 999
	JSONsupport::outputNumber(m8DevJSONoutputIdx, min(999000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 3);

	JSONsupport::closeElement(m8DevJSONoutputIdx);

	JSONsupport::closeElement(m8DevJSONoutputIdx); // end JSON payload, and go trigger read on python.script

} // end sendJSON function

#endif // defined(useJSONoutput)
