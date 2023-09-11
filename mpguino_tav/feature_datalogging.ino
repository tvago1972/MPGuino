#if defined(useDataLoggingOutput)
static void doOutputDataLog(void)
{

	uint8_t c = ',';

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		// perform the required decimal formatting, and output the number
		text::tripFunctionOut(devIdxLogOutput, pgm_read_word(&dataLogTripCalcFormats[(uint16_t)(x)]), 0, (dfOverflow9s));
		text::charOut(devIdxLogOutput, c);

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
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,		// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrShiftRegRight, 0x02,							// shift result right one bit
	instrJump, tCalculateFuelDistance					// go format it
};

#if defined(useDragRaceFunction)
static void doOutputJSONnumber(uint32_t an, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::init64((union union_64 *)(&s64reg[(uint16_t)(s64reg64_2)]), an);
	text::stringOut(devIdxJSONoutput, ull2str(nBuff, decimalPlaces, 0, (dfOverflow9s))); // output the number part
	text::stringOut(devIdxJSONoutput, labelStr); // output the label part

}

#endif // defined(useDragRaceFunction)
static void doOutputJSONfuelEcon(const char * prefixStr, uint8_t tripIdx)
{

	translateCalcIdx(tripIdx, tFuelEcon, 4, (dfOverflow9s));

	text::stringOut(devIdxJSONoutput, prefixStr);

	if (mainCalcFuncVar.value > 9999999)
	{

		text::stringOut(devIdxJSONoutput, PSTR("infinite "));
		text::stringOut(devIdxJSONoutput, mainCalcFuncVar.calcFormatLabelPtr);

	}
	else text::numberOut(devIdxJSONoutput, (dfOutputLabel));

}

static void doOutputJSONremainingFuel(void)
{

	translateCalcIdx(0, tReserveRemainingFuel, 4, (dfOverflow9s)); // reserve remaining fuel left

	if (mainCalcFuncVar.value == 0) text::stringOut(devIdxJSONoutput, PSTR("fumes")); // fumes left
	else text::numberOut(devIdxJSONoutput, (dfOutputLabel)); // reserve remaining fuel left

	text::stringOut(devIdxJSONoutput, PSTR(" remaining, "));
	text::tripFunctionOut(devIdxJSONoutput, 0, tBingoRemainingFuel, 0, (dfOverflow9s | dfOutputLabel)); // bingo remaining fuel left
	text::stringOut(devIdxJSONoutput, PSTR(" e-reserve"));

}

static void doOutputJSON(void) //skybolt added JSON output function
{

	static uint8_t subtitleCount1 = 2;
#if defined(useDragRaceFunction)
	static uint8_t subtitleCount2 = 3;

	uint32_t targetSpeed;
	uint32_t targetDistance;
#endif // defined(useDragRaceFunction)

	if (heart::testAndResetBitFlagBit(bfTimer0Status, t0sJSONchangeSubtitle)) // replaced timerChecker with this because it's a more accurate method to change once every 1.6 seconds
	{

		if (!(--subtitleCount1)) subtitleCount1 = 2;
#if defined(useDragRaceFunction)
		if (!(--subtitleCount2)) subtitleCount2 = 3;
#endif // defined(useDragRaceFunction)

	}

	JSONsupport::init(devIdxJSONoutput); // begin JSON payload

	JSONsupport::openElement(devIdxJSONoutput, JSONflagArray);

	// first graph, fuel

	JSONsupport::openElement(devIdxJSONoutput, JSONflagObject);

	JSONsupport::openKey(devIdxJSONoutput, JSONtitleStr, JSONflagString);
	doOutputJSONremainingFuel();

	// text::tripFunctionOut(devIdxJSONoutput, instantIdx, tEngineSpeed, 0, (dfOverflow9s | dfOutputLabel)); // rpm to test latency only vs tachometer and LCD vs raspi indicator (expect 2x looptime)

	JSONsupport::openKey(devIdxJSONoutput, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			text::stringOut(devIdxJSONoutput, PSTR("fuel used: "));
			text::stringOut(devIdxJSONoutput, tripStr);
			text::tripFunctionOut(devIdxJSONoutput, currentIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // current trip fuel used
			text::stringOut(devIdxJSONoutput, semicolonTankStr);
			text::tripFunctionOut(devIdxJSONoutput, tankIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // tank trip fuel used
			text::stringOut(devIdxJSONoutput, semicolonUsingStr);
			text::tripFunctionOut(devIdxJSONoutput, instantIdx, tFuelRate, 0, (dfOverflow9s | dfOutputLabel)); // current rate of fuel burn in units/time

			break;

		case 1:
			text::stringOut(devIdxJSONoutput, PSTR("eco stats: "));
#if defined(trackIdleEOCdata)
			text::stringOut(devIdxJSONoutput, PSTR("used@idle "));
			text::tripFunctionOut(devIdxJSONoutput, eocIdleTankIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel));
			text::stringOut(devIdxJSONoutput, PSTR(", fuel cut "));
			text::tripFunctionOut(devIdxJSONoutput, eocIdleTankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel));
#else // defined(trackIdleEOCdata)
			doOutputJSONremainingFuel();
#endif // defined(trackIdleEOCdata)

			break;

	}

	//ranges do not have to be in order, d3js libraries will auto sort, so you can make it easier to read the code by changing the order
	JSONsupport::openKey(devIdxJSONoutput, JSONrangesStr, JSONflagArray);
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tFuelQuantity); // largest, full tank size (e.g, 13.8 g)
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tReserveQuantity); // full tank less reserve (e.g.13.8g - 2.2g = 11.6g)
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tBingoQuantity); // reserve amount (e.g. 2.2g)

	JSONsupport::openKey(devIdxJSONoutput, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tReserveRemainingFuel); // reserve remaining fuel left
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tRemainingFuel); // total remaining fuel left

	JSONsupport::openKey(devIdxJSONoutput, JSONmarkersStr, JSONflagArray);
	JSONsupport::outputNumber(devIdxJSONoutput, instantIdx, tFuelRate); // current rate of fuel burn in units/time

	JSONsupport::closeElement(devIdxJSONoutput);

	// second graph, distance
	JSONsupport::openElement(devIdxJSONoutput, JSONflagObject);

	JSONsupport::openKey(devIdxJSONoutput, JSONtitleStr, JSONflagString);
	text::tripFunctionOut(devIdxJSONoutput, tankIdx, tReserveDistanceToEmpty, 0, (dfOverflow9s | dfOutputLabel)); // distance to bingo
	text::stringOut(devIdxJSONoutput, PSTR(" to e, "));
	text::tripFunctionOut(devIdxJSONoutput, tankIdx, tBingoDistanceToEmpty, 0, (dfOverflow9s | dfOutputLabel)); // distance to fully empty tank from bingo
	text::stringOut(devIdxJSONoutput, PSTR(" e-reserve"));

	JSONsupport::openKey(devIdxJSONoutput, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			//	text::stringOut(devIdxJSONoutput, PSTR("trip/tank distance: "));
			//	text::tripFunctionOut(devIdxJSONoutput, currentIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			//	text::stringOut(devIdxJSONoutput, PSTR("/"));
			//	text::tripFunctionOut(devIdxJSONoutput, tankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance

			text::tripFunctionOut(devIdxJSONoutput, currentIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			text::stringOut(devIdxJSONoutput, PSTR(" trip distance, "));
			text::tripFunctionOut(devIdxJSONoutput, tankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			text::stringOut(devIdxJSONoutput, PSTR(" tank distance"));
			break;

		case 1:
			text::tripFunctionOut(devIdxJSONoutput, tankIdx, tReserveDistance, 0, (dfOverflow9s | dfOutputLabel)); // reserve range
			text::stringOut(devIdxJSONoutput, PSTR(" safe range, "));
			text::tripFunctionOut(devIdxJSONoutput, tankIdx, tRangeDistance, 0, (dfOverflow9s | dfOutputLabel)); // distance to fully empty tank
			text::stringOut(devIdxJSONoutput, PSTR(" dry range"));
			break;

	}

	JSONsupport::openKey(devIdxJSONoutput, JSONrangesStr, JSONflagArray);
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tRangeDistance); // maximum range
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tReserveDistance); // range 2, safe range
	JSONsupport::outputNumber(devIdxJSONoutput, prgmFindHalfReserveRange, tankIdx, 1); // range 3, half of safe range

	JSONsupport::openKey(devIdxJSONoutput, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tBingoDistance); // shows miles of e-reserve in bar form

	JSONsupport::openKey(devIdxJSONoutput, JSONmarkersStr, JSONflagArray);
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tDistanceToEmpty); // line is distance to empty

	JSONsupport::closeElement(devIdxJSONoutput);

	// third graph, econ

	JSONsupport::openElement(devIdxJSONoutput, JSONflagObject);

	JSONsupport::openKey(devIdxJSONoutput, JSONtitleStr, JSONflagString);
#if defined(useDragRaceFunction)

	if (accelTestState) //display if we have encountered a state change in the drag test
	{

		text::stringOut(devIdxJSONoutput, findStr(JSONaccelTestStateMsgs, accelTestState));
		text::stringOut(devIdxJSONoutput, PSTR(" ..."));

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

	JSONsupport::openKey(devIdxJSONoutput, JSONsubtitleStr, JSONflagString);
#if defined(useDragRaceFunction)
	targetSpeed = SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx); // accel test speed
	targetDistance = SWEET64::runPrgm(prgmFetchParameterValue, pDragDistanceIdx); // accel test distance

	switch (subtitleCount2)
	{

		case 3:
			//1 & 2 seconds display
			text::stringOut(devIdxJSONoutput, PSTR("accel time: 0-"));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR("/"));
			doOutputJSONnumber(targetSpeed, 0, PSTR(": "));

			text::tripFunctionOut(devIdxJSONoutput, dragHalfSpeedIdx, tAccelTestTime, 0, dfOverflow9s); // 0-(half speed) time
			text::stringOut(devIdxJSONoutput, PSTR("/"));
			text::tripFunctionOut(devIdxJSONoutput, dragFullSpeedIdx, tAccelTestTime, 0, dfOverflow9s); // 0-(full speed) time
			text::stringOut(devIdxJSONoutput, PSTR(", "));
			text::tripFunctionOut(devIdxJSONoutput, dragDistanceIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // trap distance
			text::stringOut(devIdxJSONoutput, PSTR(" in "));
			text::tripFunctionOut(devIdxJSONoutput, dragDistanceIdx, tAccelTestTime, 0, (dfOverflow9s | dfOutputLabel)); // trap time
			text::stringOut(devIdxJSONoutput, PSTR(" @"));
			text::tripFunctionOut(devIdxJSONoutput, dragDistanceIdx, tTrapSpeed, 0, (dfOverflow9s | dfOutputLabel)); // trap speed
			text::stringOut(devIdxJSONoutput, PSTR("; "));
			text::tripFunctionOut(devIdxJSONoutput, dragDistanceIdx, tEstimatedEnginePower, 0, (dfOverflow9s | dfOutputLabel)); // estimated engine power
			text::stringOut(devIdxJSONoutput, PSTR(" @"));
			text::tripFunctionOut(devIdxJSONoutput, dragDistanceIdx, tDragSpeed, 0, (dfOverflow9s | dfOutputLabel)); // max speed
			break;

		case 2:
			text::stringOut(devIdxJSONoutput, PSTR("accel fuel: "));
			text::tripFunctionOut(devIdxJSONoutput, dragHalfSpeedIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // 0-(half speed) fuel
			text::stringOut(devIdxJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			text::tripFunctionOut(devIdxJSONoutput, dragFullSpeedIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // 0-(full speed) fuel
			text::stringOut(devIdxJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			text::tripFunctionOut(devIdxJSONoutput, dragDistanceIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // trap fuel
			text::stringOut(devIdxJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

		case 1:
			text::stringOut(devIdxJSONoutput, PSTR("accel fuel: "));

			text::tripFunctionOut(devIdxJSONoutput, dragHalfSpeedIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // 0-(half speed) mpg
			text::stringOut(devIdxJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			text::tripFunctionOut(devIdxJSONoutput, dragFullSpeedIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // 0-(full speed) mpg
			text::stringOut(devIdxJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			text::tripFunctionOut(devIdxJSONoutput, dragDistanceIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // trap mpg
			text::stringOut(devIdxJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

	}
#else // defined(useDragRaceFunction)
	text::stringOut(devIdxJSONoutput, PSTR("[this space intentionally left blank]"));
#endif // defined(useDragRaceFunction)

	JSONsupport::openKey(devIdxJSONoutput, JSONrangesStr, JSONflagArray);
	// set scale at 40mpg or instant econ up to 999 mpg. Folks like to watch their mpg meter go to extremes
	JSONsupport::outputNumber(devIdxJSONoutput, 18000ul, 3);
	JSONsupport::outputNumber(devIdxJSONoutput, 24000ul, 3);
	JSONsupport::outputNumber(devIdxJSONoutput, min(max(40000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 999000), 3);

	JSONsupport::openKey(devIdxJSONoutput, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(devIdxJSONoutput, currentIdx, tFuelEcon); // current fuel economy
	JSONsupport::outputNumber(devIdxJSONoutput, tankIdx, tFuelEcon); // tank fuel economy

	JSONsupport::openKey(devIdxJSONoutput, JSONmarkersStr, JSONflagArray);
	// instantaneous fuel economy, do not let scale exceed 999
	JSONsupport::outputNumber(devIdxJSONoutput, min(999000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 3);

	JSONsupport::closeElement(devIdxJSONoutput);

	JSONsupport::closeElement(devIdxJSONoutput); // end JSON payload, and go trigger read on python.script

} // end sendJSON function

#endif // defined(useJSONoutput)
