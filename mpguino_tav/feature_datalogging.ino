#if defined(useDataLoggingOutput)
static void doOutputDataLog(void)
{

	uint8_t c = ',';

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		// perform the required decimal formatting, and output the number
		text::tripFunctionOut(devLogOutput, pgm_read_word(&dataLogTripCalcFormats[(uint16_t)(x)]), 0, (dfOverflow9s));
		text::charOut(devLogOutput, c);

	}

}

#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
/* JSON formatting support section */

static void JSONsupport::init(interfaceDevice &dev)
{

	JSONlevel = 0;
	JSONflag[(uint16_t)(JSONlevel)] = 0;

	text::stringOut(dev, PSTR("\n\n")); // begin JSON payload

}

static void JSONsupport::openElement(interfaceDevice &dev, uint8_t elementType)
{

	outputElementNext(dev, elementType);

	JSONflag[(uint16_t)(JSONlevel)] |= (elementType);

	JSONlevel++;
	JSONflag[(uint16_t)(JSONlevel)] = 0;

	outputElementStart(dev, elementType);

}

static void JSONsupport::openKey(interfaceDevice &dev, const char * str, uint8_t elementType)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) closeElementInternal(dev);

	openElement(dev, 0);

	text::charOut(dev, '"');
	text::stringOut(dev, str);
	text::stringOut(dev, PSTR("\":"));

	JSONflag[(uint16_t)(JSONlevel)] |= (elementType | JSONflagKey);

	outputElementStart(dev, elementType);

}

static void JSONsupport::closeElement(interfaceDevice &dev)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) closeElementInternal(dev);

	closeElementInternal(dev);

}

static void JSONsupport::closeElementInternal(interfaceDevice &dev)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) outputElementEnd(dev);

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagString) text::charOut(dev, '"');

	JSONlevel--;

	outputElementEnd(dev);

	if (JSONlevel == 0) text::stringOut(dev, PSTR("\n\n")); // end JSON payload, and go trigger read on python.script

}

static void JSONsupport::outputElementStart(interfaceDevice &dev, uint8_t elementType)
{

	if (elementType & JSONflagArray) text::charOut(dev, '[');

	if (elementType & JSONflagObject) text::charOut(dev, '{');

	if (elementType & JSONflagString) text::charOut(dev, '"');

}

static void JSONsupport::outputElementNext(interfaceDevice &dev, uint8_t elementType)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagString)
	{

		text::charOut(dev, '"');
		JSONflag[(uint16_t)(JSONlevel)] &= ~(JSONflagString);

	}

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagMany) text::charOut(dev, ',');

	JSONflag[(uint16_t)(JSONlevel)] |= (JSONflagMany);

	if (elementType & JSONflagString)
	{

		text::charOut(dev, '"');
		JSONflag[(uint16_t)(JSONlevel)] |= (JSONflagString);

	}

}

static void JSONsupport::outputElementEnd(interfaceDevice &dev)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagArray) text::charOut(dev, ']');

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagObject) text::charOut(dev, '}');

}

static void JSONsupport::outputNumber(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx)
{

	outputElementNext(dev, 0);
	text::tripFunctionOut(dev, tripIdx, calcIdx, 0, (dfOverflow9s));

}

static void JSONsupport::outputNumber(interfaceDevice &dev, const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces)
{

	outputElementNext(dev, 0);
	SWEET64::runPrgm(sched, tripIdx);
	text::stringOut(dev, ull2str(nBuff, decimalPlaces, 0, (dfOverflow9s))); // output the number part

}

static void JSONsupport::outputNumber(interfaceDevice &dev, uint32_t an, uint8_t decimalPlaces)
{

	outputElementNext(dev, 0);
	SWEET64::init64((union union_64 *)(&s64reg[s64reg2]), an);
	text::stringOut(dev, ull2str(nBuff, decimalPlaces, 0, (dfOverflow9s))); // output the number part

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

	SWEET64::init64((union union_64 *)(&s64reg[s64reg2]), an);
	text::stringOut(devJSONoutput, ull2str(nBuff, decimalPlaces, 0, (dfOverflow9s))); // output the number part
	text::stringOut(devJSONoutput, labelStr); // output the label part

}

#endif // defined(useDragRaceFunction)
static void doOutputJSONfuelEcon(const char * prefixStr, uint8_t tripIdx)
{

	translateCalcIdx(tripIdx, tFuelEcon, 4, (dfOverflow9s));

	text::stringOut(devJSONoutput, prefixStr);

	if (mainCalcFuncVar.value > 9999999)
	{

		text::stringOut(devJSONoutput, PSTR("infinite "));
		text::stringOut(devJSONoutput, mainCalcFuncVar.calcFormatLabelPtr);

	}
	else text::numberOut(devJSONoutput, (dfOutputLabel));

}

static void doOutputJSONremainingFuel(void)
{

	translateCalcIdx(0, tReserveRemainingFuel, 4, (dfOverflow9s)); // reserve remaining fuel left

	if (mainCalcFuncVar.value == 0) text::stringOut(devJSONoutput, PSTR("fumes")); // fumes left
	else text::numberOut(devJSONoutput, (dfOutputLabel)); // reserve remaining fuel left

	text::stringOut(devJSONoutput, PSTR(" remaining, "));
	text::tripFunctionOut(devJSONoutput, 0, tBingoRemainingFuel, 0, (dfOverflow9s | dfOutputLabel)); // bingo remaining fuel left
	text::stringOut(devJSONoutput, PSTR(" e-reserve"));

}

static void doOutputJSON(void) //skybolt added JSON output function
{

	static uint8_t subtitleCount1 = 2;
#if defined(useDragRaceFunction)
	static uint8_t subtitleCount2 = 3;

	uint32_t targetSpeed;
	uint32_t targetDistance;
#endif // defined(useDragRaceFunction)

	if (timer0Status & t0sJSONchangeSubtitle) // replaced timerChecker with this because it's a more accurate method to change once every 1.6 seconds
	{

		heart::changeBitFlags(timer0Status, t0sJSONchangeSubtitle, 0); // clear JSON subtitle change timer command

		if (!(--subtitleCount1)) subtitleCount1 = 2;
#if defined(useDragRaceFunction)
		if (!(--subtitleCount2)) subtitleCount2 = 3;
#endif // defined(useDragRaceFunction)

	}

	JSONsupport::init(devJSONoutput); // begin JSON payload

	JSONsupport::openElement(devJSONoutput, JSONflagArray);

	// first graph, fuel

	JSONsupport::openElement(devJSONoutput, JSONflagObject);

	JSONsupport::openKey(devJSONoutput, JSONtitleStr, JSONflagString);
	doOutputJSONremainingFuel();

	// text::tripFunctionOut(devJSONoutput, instantIdx, tEngineSpeed, 0, (dfOverflow9s | dfOutputLabel)); // rpm to test latency only vs tachometer and LCD vs raspi indicator (expect 2x looptime)

	JSONsupport::openKey(devJSONoutput, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			text::stringOut(devJSONoutput, PSTR("fuel used: "));
			text::stringOut(devJSONoutput, tripStr);
			text::tripFunctionOut(devJSONoutput, currentIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // current trip fuel used
			text::stringOut(devJSONoutput, semicolonTankStr);
			text::tripFunctionOut(devJSONoutput, tankIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // tank trip fuel used
			text::stringOut(devJSONoutput, semicolonUsingStr);
			text::tripFunctionOut(devJSONoutput, instantIdx, tFuelRate, 0, (dfOverflow9s | dfOutputLabel)); // current rate of fuel burn in units/time

			break;

		case 1:
			text::stringOut(devJSONoutput, PSTR("eco stats: "));
#if defined(trackIdleEOCdata)
			text::stringOut(devJSONoutput, PSTR("used@idle "));
			text::tripFunctionOut(devJSONoutput, eocIdleTankIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel));
			text::stringOut(devJSONoutput, PSTR(", fuel cut "));
			text::tripFunctionOut(devJSONoutput, eocIdleTankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel));
#else // defined(trackIdleEOCdata)
			doOutputJSONremainingFuel();
#endif // defined(trackIdleEOCdata)

			break;

	}

	//ranges do not have to be in order, d3js libraries will auto sort, so you can make it easier to read the code by changing the order
	JSONsupport::openKey(devJSONoutput, JSONrangesStr, JSONflagArray);
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tFuelQuantity); // largest, full tank size (e.g, 13.8 g)
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tReserveQuantity); // full tank less reserve (e.g.13.8g - 2.2g = 11.6g)
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tBingoQuantity); // reserve amount (e.g. 2.2g)

	JSONsupport::openKey(devJSONoutput, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tReserveRemainingFuel); // reserve remaining fuel left
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tRemainingFuel); // total remaining fuel left

	JSONsupport::openKey(devJSONoutput, JSONmarkersStr, JSONflagArray);
	JSONsupport::outputNumber(devJSONoutput, instantIdx, tFuelRate); // current rate of fuel burn in units/time

	JSONsupport::closeElement(devJSONoutput);

	// second graph, distance
	JSONsupport::openElement(devJSONoutput, JSONflagObject);

	JSONsupport::openKey(devJSONoutput, JSONtitleStr, JSONflagString);
	text::tripFunctionOut(devJSONoutput, tankIdx, tReserveDistanceToEmpty, 0, (dfOverflow9s | dfOutputLabel)); // distance to bingo
	text::stringOut(devJSONoutput, PSTR(" to e, "));
	text::tripFunctionOut(devJSONoutput, tankIdx, tBingoDistanceToEmpty, 0, (dfOverflow9s | dfOutputLabel)); // distance to fully empty tank from bingo
	text::stringOut(devJSONoutput, PSTR(" e-reserve"));

	JSONsupport::openKey(devJSONoutput, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			//	text::stringOut(devJSONoutput, PSTR("trip/tank distance: "));
			//	text::tripFunctionOut(devJSONoutput, currentIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			//	text::stringOut(devJSONoutput, PSTR("/"));
			//	text::tripFunctionOut(devJSONoutput, tankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance

			text::tripFunctionOut(devJSONoutput, currentIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			text::stringOut(devJSONoutput, PSTR(" trip distance, "));
			text::tripFunctionOut(devJSONoutput, tankIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // current trip distance
			text::stringOut(devJSONoutput, PSTR(" tank distance"));
			break;

		case 1:
			text::tripFunctionOut(devJSONoutput, tankIdx, tReserveDistance, 0, (dfOverflow9s | dfOutputLabel)); // reserve range
			text::stringOut(devJSONoutput, PSTR(" safe range, "));
			text::tripFunctionOut(devJSONoutput, tankIdx, tRangeDistance, 0, (dfOverflow9s | dfOutputLabel)); // distance to fully empty tank
			text::stringOut(devJSONoutput, PSTR(" dry range"));
			break;

	}

	JSONsupport::openKey(devJSONoutput, JSONrangesStr, JSONflagArray);
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tRangeDistance); // maximum range
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tReserveDistance); // range 2, safe range
	JSONsupport::outputNumber(devJSONoutput, prgmFindHalfReserveRange, tankIdx, 1); // range 3, half of safe range

	JSONsupport::openKey(devJSONoutput, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tBingoDistance); // shows miles of e-reserve in bar form

	JSONsupport::openKey(devJSONoutput, JSONmarkersStr, JSONflagArray);
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tDistanceToEmpty); // line is distance to empty

	JSONsupport::closeElement(devJSONoutput);

	// third graph, econ

	JSONsupport::openElement(devJSONoutput, JSONflagObject);

	JSONsupport::openKey(devJSONoutput, JSONtitleStr, JSONflagString);
#if defined(useDragRaceFunction)

	if (accelTestState) //display if we have encountered a state change in the drag test
	{

		text::stringOut(devJSONoutput, findStr(JSONaccelTestStateMsgs, accelTestState));
		text::stringOut(devJSONoutput, PSTR(" ..."));

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

	JSONsupport::openKey(devJSONoutput, JSONsubtitleStr, JSONflagString);
#if defined(useDragRaceFunction)
	targetSpeed = SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx); // accel test speed
	targetDistance = SWEET64::runPrgm(prgmFetchParameterValue, pDragDistanceIdx); // accel test distance

	switch (subtitleCount2)
	{

		case 3:
			//1 & 2 seconds display
			text::stringOut(devJSONoutput, PSTR("accel time: 0-"));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR("/"));
			doOutputJSONnumber(targetSpeed, 0, PSTR(": "));

			text::tripFunctionOut(devJSONoutput, dragHalfSpeedIdx, tAccelTestTime, 0, dfOverflow9s); // 0-(half speed) time
			text::stringOut(devJSONoutput, PSTR("/"));
			text::tripFunctionOut(devJSONoutput, dragFullSpeedIdx, tAccelTestTime, 0, dfOverflow9s); // 0-(full speed) time
			text::stringOut(devJSONoutput, PSTR(", "));
			text::tripFunctionOut(devJSONoutput, dragDistanceIdx, tDistance, 0, (dfOverflow9s | dfOutputLabel)); // trap distance
			text::stringOut(devJSONoutput, PSTR(" in "));
			text::tripFunctionOut(devJSONoutput, dragDistanceIdx, tAccelTestTime, 0, (dfOverflow9s | dfOutputLabel)); // trap time
			text::stringOut(devJSONoutput, PSTR(" @"));
			text::tripFunctionOut(devJSONoutput, dragDistanceIdx, tTrapSpeed, 0, (dfOverflow9s | dfOutputLabel)); // trap speed
			text::stringOut(devJSONoutput, PSTR("; "));
			text::tripFunctionOut(devJSONoutput, dragDistanceIdx, tEstimatedEnginePower, 0, (dfOverflow9s | dfOutputLabel)); // estimated engine power
			text::stringOut(devJSONoutput, PSTR(" @"));
			text::tripFunctionOut(devJSONoutput, dragDistanceIdx, tDragSpeed, 0, (dfOverflow9s | dfOutputLabel)); // max speed
			break;

		case 2:
			text::stringOut(devJSONoutput, PSTR("accel fuel: "));
			text::tripFunctionOut(devJSONoutput, dragHalfSpeedIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // 0-(half speed) fuel
			text::stringOut(devJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			text::tripFunctionOut(devJSONoutput, dragFullSpeedIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // 0-(full speed) fuel
			text::stringOut(devJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			text::tripFunctionOut(devJSONoutput, dragDistanceIdx, tFuelUsed, 0, (dfOverflow9s | dfOutputLabel)); // trap fuel
			text::stringOut(devJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

		case 1:
			text::stringOut(devJSONoutput, PSTR("accel fuel: "));

			text::tripFunctionOut(devJSONoutput, dragHalfSpeedIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // 0-(half speed) mpg
			text::stringOut(devJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			text::tripFunctionOut(devJSONoutput, dragFullSpeedIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // 0-(full speed) mpg
			text::stringOut(devJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			text::tripFunctionOut(devJSONoutput, dragDistanceIdx, tFuelEcon, 0, (dfOverflow9s | dfOutputLabel)); // trap mpg
			text::stringOut(devJSONoutput, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

	}
#else // defined(useDragRaceFunction)
	text::stringOut(devJSONoutput, PSTR("[this space intentionally left blank]"));
#endif // defined(useDragRaceFunction)

	JSONsupport::openKey(devJSONoutput, JSONrangesStr, JSONflagArray);
	// set scale at 40mpg or instant econ up to 999 mpg. Folks like to watch their mpg meter go to extremes
	JSONsupport::outputNumber(devJSONoutput, 18000ul, 3);
	JSONsupport::outputNumber(devJSONoutput, 24000ul, 3);
	JSONsupport::outputNumber(devJSONoutput, min(max(40000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 999000), 3);

	JSONsupport::openKey(devJSONoutput, JSONmeasuresStr, JSONflagArray);
	JSONsupport::outputNumber(devJSONoutput, currentIdx, tFuelEcon); // current fuel economy
	JSONsupport::outputNumber(devJSONoutput, tankIdx, tFuelEcon); // tank fuel economy

	JSONsupport::openKey(devJSONoutput, JSONmarkersStr, JSONflagArray);
	// instantaneous fuel economy, do not let scale exceed 999
	JSONsupport::outputNumber(devJSONoutput, min(999000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 3);

	JSONsupport::closeElement(devJSONoutput);

	JSONsupport::closeElement(devJSONoutput); // end JSON payload, and go trigger read on python.script

} // end sendJSON function

#endif // defined(useJSONoutput)
