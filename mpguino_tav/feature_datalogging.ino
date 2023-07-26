#if defined(useDataLoggingOutput)
static void doOutputDataLog(void)
{

	calcFuncObj thisCalcFuncObj;

	uint8_t c = ',';

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		// perform the required decimal formatting
		thisCalcFuncObj = translateCalcIdx(pgm_read_word(&dataLogTripCalcFormats[(uint16_t)(x)]), nBuff, 0, dfOverflow9s);

		text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer); // output the number
		text::charOut(devLogOutput, c);

	}

}

#endif // defined(useDataLoggingOutput)
#if defined(useJSONoutput)
/* JSON formatting support section */

static void JSON::init(interfaceDevice &dev)
{

	JSONlevel = 0;
	JSONflag[(uint16_t)(JSONlevel)] = 0;

	text::stringOut(dev, PSTR("\n\n")); // begin JSON payload

}

static void JSON::openElement(interfaceDevice &dev, uint8_t elementType)
{

	outputElementNext(dev, elementType);

	JSONflag[(uint16_t)(JSONlevel)] |= (elementType);

	JSONlevel++;
	JSONflag[(uint16_t)(JSONlevel)] = 0;

	outputElementStart(dev, elementType);

}

static void JSON::openKey(interfaceDevice &dev, const char * str, uint8_t elementType)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) closeElementInternal(dev);

	openElement(dev, 0);

	text::charOut(dev, '"');
	text::stringOut(dev, str);
	text::stringOut(dev, PSTR("\":"));

	JSONflag[(uint16_t)(JSONlevel)] |= (elementType | JSONflagKey);

	outputElementStart(dev, elementType);

}

static void JSON::closeElement(interfaceDevice &dev)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) closeElementInternal(dev);

	closeElementInternal(dev);

}

static void JSON::closeElementInternal(interfaceDevice &dev)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagKey) outputElementEnd(dev);

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagString) text::charOut(dev, '"');

	JSONlevel--;

	outputElementEnd(dev);

	if (JSONlevel == 0) text::stringOut(dev, PSTR("\n\n")); // end JSON payload, and go trigger read on python.script

}

static void JSON::outputElementStart(interfaceDevice &dev, uint8_t elementType)
{

	if (elementType & JSONflagArray) text::charOut(dev, '[');

	if (elementType & JSONflagObject) text::charOut(dev, '{');

	if (elementType & JSONflagString) text::charOut(dev, '"');

}

static void JSON::outputElementNext(interfaceDevice &dev, uint8_t elementType)
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

static void JSON::outputElementEnd(interfaceDevice &dev)
{

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagArray) text::charOut(dev, ']');

	if (JSONflag[(uint16_t)(JSONlevel)] & JSONflagObject) text::charOut(dev, '}');

}

static void JSON::outputNumber(interfaceDevice &dev, uint8_t tripIdx, uint8_t calcIdx)
{

	calcFuncObj thisCalcFuncObj;

	// perform the required decimal formatting
	thisCalcFuncObj = translateCalcIdx(tripIdx, calcIdx, nBuff, 0, dfOverflow9s);

	outputElementNext(dev, 0);
	text::stringOut(dev, thisCalcFuncObj.strBuffer); // output the number

}

static void JSON::outputNumber(interfaceDevice &dev, const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces)
{

	outputElementNext(dev, 0);
	SWEET64::runPrgm(sched, tripIdx);
	text::stringOut(dev, ull2str(nBuff, decimalPlaces, 0, dfOverflow9s)); // output the number part

}

static void JSON::outputNumber(interfaceDevice &dev, union union_64 * an, uint8_t decimalPlaces)
{

	outputElementNext(dev, 0);
	SWEET64::copy64((union union_64 *)(&s64reg[s64reg2]), an);
	text::stringOut(dev, ull2str(nBuff, decimalPlaces, 0, dfOverflow9s)); // output the number part

}

static void JSON::outputNumber(interfaceDevice &dev, uint32_t an, uint8_t decimalPlaces)
{

	outputElementNext(dev, 0);
	SWEET64::init64((union union_64 *)(&s64reg[s64reg2]), an);
	text::stringOut(dev, ull2str(nBuff, decimalPlaces, 0, dfOverflow9s)); // output the number part

}

/* skybolt JSON packet output support section */

static const uint8_t prgmFindHalfReserveRange[] PROGMEM = {
	instrLdRegMain, 0x02, mpTankSizeIdx,				// fetch calculated tank size in injector open cycles
	instrSubMainFromX, 0x02, mpBingoTankSizeIdx,			// subtract bingo fuel value in cycles from remaining fuel in cycles to get reserve fuel in cycles
	instrShiftRegRight, 0x02,							// shift result right one bit
	instrJump, tCalculateFuelDistance					// go format it
};

void doOutputJSONnumber(unsigned long an, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::init64((union union_64 *)(&s64reg[s64reg2]), an);
	text::stringOut(devLogOutput, ull2str(nBuff, decimalPlaces, 0, dfOverflow9s)); // output the number part
	text::stringOut(devLogOutput, labelStr); // output the label part

}

void doOutputJSON(void) //skybolt added json output function
{

	calcFuncObj thisCalcFuncObj;

	static uint8_t subtitleCount1 = 2;
#if defined(useDragRaceFunction)
	static uint8_t subtitleCount2 = 3;

	uint32_t targetSpeed;
	uint32_t targetDistance;
#endif // defined(useDragRaceFunction)

	if (timer0Status & t0sOutputJSON) // replaced timerChecker with this because systemInfo::cycles0() is a compile-time option that could break JSON output if #useCPUreading is not set
	{

		changeBitFlags(timer0Status, t0sOutputJSON, 0); // clear JSON subtitle change timer command

		if (!(--subtitleCount1)) subtitleCount1 = 2;
#if defined(useDragRaceFunction)
		if (!(--subtitleCount2)) subtitleCount2 = 3;
#endif // defined(useDragRaceFunction)

	}

	JSON::init(devLogOutput); // begin JSON payload

	JSON::openElement(devLogOutput, JSONflagArray);

	// first graph, fuel

	JSON::openElement(devLogOutput, JSONflagObject);

	JSON::openKey(devLogOutput, JSONtitleStr, JSONflagString);
	thisCalcFuncObj = translateCalcIdx(0, tReserveRemainingFuel, nBuff, 4, dfOverflow9s); // reserve remaining fuel left
	if (thisCalcFuncObj.value == 0) text::stringOut(devLogOutput, PSTR("fumes")); // fumes left
	else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer); // reserve remaining fuel left
	text::stringOut(devLogOutput, PSTR(" remaining, "));
	outputTripFunctionValue(devLogOutput, 0, tBingoRemainingFuel, nBuff, 0, dfOverflow9s); // bingo remaining fuel left
	text::stringOut(devLogOutput, PSTR(" e-reserve"));

	// outputTripFunctionValue(devLogOutput, instantIdx, tEngineSpeed, nBuff, 0, dfOverflow9s); // rpm to test latency only vs tachometer and LCD vs raspi indicator (expect 2x looptime)

	JSON::openKey(devLogOutput, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			text::stringOut(devLogOutput, PSTR("fuel used: "));
			text::stringOut(devLogOutput, PSTR("trip "));
			outputTripFunctionValue(devLogOutput, currentIdx, tFuelUsed, nBuff, 0, dfOverflow9s); // current trip fuel used
			text::stringOut(devLogOutput, PSTR("; tank "));
			outputTripFunctionValue(devLogOutput, tankIdx, tFuelUsed, nBuff, 0, dfOverflow9s); // tank trip fuel used
			text::stringOut(devLogOutput, PSTR("; using "));
			outputTripFunctionValue(devLogOutput, instantIdx, tFuelRate, nBuff, 0, dfOverflow9s); // current rate of fuel burn in units/time

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("eco stats: "));
#if defined(trackIdleEOCdata)
			text::stringOut(devLogOutput, PSTR("used@idle "));
			outputTripFunctionValue(devLogOutput, eocIdleTankIdx, tFuelUsed, nBuff, 0, dfOverflow9s);
			text::stringOut(devLogOutput, PSTR(", fuel cut "));
			outputTripFunctionValue(devLogOutput, eocIdleTankIdx, tDistance, nBuff, 0, dfOverflow9s);
#else // defined(trackIdleEOCdata)
			thisCalcFuncObj = translateCalcIdx(0, tReserveRemainingFuel, nBuff, 4, dfOverflow9s); // reserve remaining fuel left
			if (thisCalcFuncObj.value == 0) text::stringOut(devLogOutput, PSTR("fumes")); // fumes left
			else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer); // reserve remaining fuel left
			text::stringOut(devLogOutput, PSTR(" remaining, "));
			outputTripFunctionValue(devLogOutput, 0, tBingoRemainingFuel, nBuff, 0, dfOverflow9s); // bingo remaining fuel left
			text::stringOut(devLogOutput, PSTR(" e-reserve"));
#endif // defined(trackIdleEOCdata)

			break;

	}

	//ranges do not have to be in order, d3js libraries will auto sort, so you can make it easier to read the code by changing the order
	JSON::openKey(devLogOutput, JSONrangesStr, JSONflagArray);
	JSON::outputNumber(devLogOutput, tankIdx, tFuelQuantity); // largest, full tank size (e.g, 13.8 g)
	JSON::outputNumber(devLogOutput, tankIdx, tReserveQuantity); // full tank less reserve (e.g.13.8g - 2.2g = 11.6g)
	JSON::outputNumber(devLogOutput, tankIdx, tBingoQuantity); // reserve amount (e.g. 2.2g)

	JSON::openKey(devLogOutput, JSONmeasuresStr, JSONflagArray);
	JSON::outputNumber(devLogOutput, tankIdx, tReserveRemainingFuel); // reserve remaining fuel left
	JSON::outputNumber(devLogOutput, tankIdx, tRemainingFuel); // total remaining fuel left

	JSON::openKey(devLogOutput, JSONmarkersStr, JSONflagArray);
	JSON::outputNumber(devLogOutput, instantIdx, tFuelRate); // current rate of fuel burn in units/time

	JSON::closeElement(devLogOutput);

	// second graph, distance
	JSON::openElement(devLogOutput, JSONflagObject);

	JSON::openKey(devLogOutput, JSONtitleStr, JSONflagString);
	outputTripFunctionValue(devLogOutput, tankIdx, tReserveDistanceToEmpty, nBuff, 0, dfOverflow9s); // distance to bingo
	text::stringOut(devLogOutput, PSTR(" to e, "));
	outputTripFunctionValue(devLogOutput, tankIdx, tBingoDistanceToEmpty, nBuff, 0, dfOverflow9s); // distance to fully empty tank from bingo
	text::stringOut(devLogOutput, PSTR(" e-reserve"));

	JSON::openKey(devLogOutput, JSONsubtitleStr, JSONflagString);
	switch (subtitleCount1)
	{

		case 2:
			//	text::stringOut(devLogOutput, PSTR("trip/tank distance: "));
			//	outputTripFunctionValue(devLogOutput, currentIdx, tDistance, nBuff, 0, dfOverflow9s); // current trip distance
			//	text::stringOut(devLogOutput, PSTR("/"));
			//	outputTripFunctionValue(devLogOutput, tankIdx, tDistance, nBuff, 0, dfOverflow9s); // current trip distance

			outputTripFunctionValue(devLogOutput, currentIdx, tDistance, nBuff, 0, dfOverflow9s); // current trip distance
			text::stringOut(devLogOutput, PSTR(" trip distance, "));
			outputTripFunctionValue(devLogOutput, tankIdx, tDistance, nBuff, 0, dfOverflow9s); // current trip distance
			text::stringOut(devLogOutput, PSTR(" tank distance"));
			break;

		case 1:
			outputTripFunctionValue(devLogOutput, tankIdx, tReserveDistance, nBuff, 0, dfOverflow9s); // reserve range
			text::stringOut(devLogOutput, PSTR(" safe range, "));
			outputTripFunctionValue(devLogOutput, tankIdx, tRangeDistance, nBuff, 0, dfOverflow9s); // distance to fully empty tank
			text::stringOut(devLogOutput, PSTR(" dry range"));
			break;

	}

	JSON::openKey(devLogOutput, JSONrangesStr, JSONflagArray);
	JSON::outputNumber(devLogOutput, tankIdx, tRangeDistance); // maximum range
	JSON::outputNumber(devLogOutput, tankIdx, tReserveDistance); // range 2, safe range
	JSON::outputNumber(devLogOutput, prgmFindHalfReserveRange, tankIdx, 1); // range 3, half of safe range

	JSON::openKey(devLogOutput, JSONmeasuresStr, JSONflagArray);
	JSON::outputNumber(devLogOutput, tankIdx, tBingoDistance); // shows miles of e-reserve in bar form

	JSON::openKey(devLogOutput, JSONmarkersStr, JSONflagArray);
	JSON::outputNumber(devLogOutput, tankIdx, tDistanceToEmpty); // line is distance to empty

	JSON::closeElement(devLogOutput);

	// third graph, econ

	JSON::openElement(devLogOutput, JSONflagObject);

	JSON::openKey(devLogOutput, JSONtitleStr, JSONflagString);
#if defined(useDragRaceFunction)
	// there was a bug in some of the drag cancel logic when using Adafruit. I disabled some statements
	// to get it to work. (Was tripping at 0.718 or 0.0718 seconds (don't remember exact digits)
	//
	// yah, the fuel injector monitor was causing the accel test to cancel prematurely. That no longer happens
	//
	//some kind of bit mask; full and half-speed seem to be backwards (0 means tripped)
	//unclear on syntax for building if/then statements
	//accelerationFlags & accelTestTriggered
	// 120, waiting
	// 184, testing.
	// 176, distance met.
	// 160, distance and half-speed met. (Full speed then meets all conditions, test ends)
	// 168 half-speed met. (not distance)
	// 136 full-speed met. (Meeting distance then meets all three conditions, race ends).
	// what the hell is 156? Did this actually come up? (it appears to be active | halfspeed | cancelled | finished)
	// lastAccelTestStatus = 120; //use to force trigger

	if (accelTestState) //display if we have encountered a state change in the drag test
	{

		text::stringOut(devLogOutput, findStr(JSONaccelTestStateMsgs, accelTestState));
		text::stringOut(devLogOutput, PSTR(" ..."));

	}
	else // else not racing or waiting, go to normal
	{

		text::stringOut(devLogOutput, PSTR("fuel economy trip/tank/inst: ")); // gal or L
		thisCalcFuncObj = translateCalcIdx(currentIdx, tFuelEcon, nBuff, 4, dfOverflow9s); // current fuel economy
		if (thisCalcFuncObj.value > 9999999) text::stringOut(devLogOutput, PSTR("infinite"));
		else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
		text::stringOut(devLogOutput, PSTR("/ "));
		thisCalcFuncObj = translateCalcIdx(tankIdx, tFuelEcon, nBuff, 4, dfOverflow9s); // tank fuel economy
		if (thisCalcFuncObj.value > 9999999) text::stringOut(devLogOutput, PSTR("infinite"));
		else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
		text::stringOut(devLogOutput, PSTR("/ "));
		thisCalcFuncObj = translateCalcIdx(instantIdx, tFuelEcon, nBuff, 4, dfOverflow9s); // instantaneous fuel economy
		if (thisCalcFuncObj.value > 9999999) text::stringOut(devLogOutput, PSTR("infinite"));
		else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
		text::stringOut(devLogOutput, PSTR(" "));
		text::stringOut(devLogOutput, thisCalcFuncObj.calcFormatLabelPtr);

	}
#else // defined(useDragRaceFunction)
	text::stringOut(devLogOutput, PSTR("fuel economy trip/tank/inst: ")); // gal or L
	thisCalcFuncObj = translateCalcIdx(currentIdx, tFuelEcon, nBuff, 4, dfOverflow9s); // current fuel economy
	if (thisCalcFuncObj.value > 9999999) text::stringOut(devLogOutput, PSTR("infinite"));
	else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
	text::stringOut(devLogOutput, PSTR("/ "));
	thisCalcFuncObj = translateCalcIdx(tankIdx, tFuelEcon, nBuff, 4, dfOverflow9s); // tank fuel economy
	if (thisCalcFuncObj.value > 9999999) text::stringOut(devLogOutput, PSTR("infinite"));
	else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
	text::stringOut(devLogOutput, PSTR("/ "));
	thisCalcFuncObj = translateCalcIdx(instantIdx, tFuelEcon, nBuff, 4, dfOverflow9s); // instantaneous fuel economy
	if (thisCalcFuncObj.value > 9999999) text::stringOut(devLogOutput, PSTR("infinite"));
	else text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
	text::stringOut(devLogOutput, PSTR(" "));
	text::stringOut(devLogOutput, thisCalcFuncObj.calcFormatLabelPtr);
#endif // defined(useDragRaceFunction)

	JSON::openKey(devLogOutput, JSONsubtitleStr, JSONflagString);
#if defined(useDragRaceFunction)
	targetSpeed = SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx); // accel test speed
	targetDistance = SWEET64::runPrgm(prgmFetchParameterValue, pDragDistanceIdx); // accel test distance

	switch (subtitleCount2)
	{

		case 3:
			//1 & 2 seconds display
			text::stringOut(devLogOutput, PSTR("accel time: 0-"));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR("/"));
			doOutputJSONnumber(targetSpeed, 0, PSTR(": "));

			outputTripFunctionValue(devLogOutput, dragHalfSpeedIdx, tAccelTestTime, nBuff, 0, (dfSuppressLabel | dfOverflow9s)); // 0-(half speed) time
			text::stringOut(devLogOutput, PSTR("/"));
			outputTripFunctionValue(devLogOutput, dragFullSpeedIdx, tAccelTestTime, nBuff, 0, (dfSuppressLabel | dfOverflow9s)); // 0-(full speed) time
			text::stringOut(devLogOutput, PSTR(", "));
			outputTripFunctionValue(devLogOutput, dragDistanceIdx, tDistance, nBuff, 0, dfOverflow9s); // trap distance
			text::stringOut(devLogOutput, PSTR(" in "));
			outputTripFunctionValue(devLogOutput, dragDistanceIdx, tAccelTestTime, nBuff, 0, dfOverflow9s); // trap time
			text::stringOut(devLogOutput, PSTR(" @"));
			outputTripFunctionValue(devLogOutput, dragDistanceIdx, tTrapSpeed, nBuff, 0, dfOverflow9s); // trap speed
			text::stringOut(devLogOutput, PSTR("; "));
			outputTripFunctionValue(devLogOutput, dragDistanceIdx, tEstimatedEnginePower, nBuff, 0, dfOverflow9s); // estimated engine power
			text::stringOut(devLogOutput, PSTR(" @"));
			outputTripFunctionValue(devLogOutput, dragDistanceIdx, tDragSpeed, nBuff, 0, dfOverflow9s); // max speed
			break;

		case 2:
			text::stringOut(devLogOutput, PSTR("accel fuel: "));
			outputTripFunctionValue(devLogOutput, dragHalfSpeedIdx, tFuelUsed, nBuff, 0, dfOverflow9s); // 0-(half speed) fuel
			text::stringOut(devLogOutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			outputTripFunctionValue(devLogOutput, dragFullSpeedIdx, tFuelUsed, nBuff, 0, dfOverflow9s); // 0-(full speed) fuel
			text::stringOut(devLogOutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			outputTripFunctionValue(devLogOutput, dragDistanceIdx, tFuelUsed, nBuff, 0, dfOverflow9s); // trap fuel
			text::stringOut(devLogOutput, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("accel fuel: "));

			outputTripFunctionValue(devLogOutput, dragHalfSpeedIdx, tFuelEcon, nBuff, 0, dfOverflow9s); // 0-(half speed) mpg
			text::stringOut(devLogOutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed / 2, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(half speed)

			outputTripFunctionValue(devLogOutput, dragFullSpeedIdx, tFuelEcon, nBuff, 0, dfOverflow9s); // 0-(full speed) mpg
			text::stringOut(devLogOutput, PSTR(" to "));

			doOutputJSONnumber(targetSpeed, 0, PSTR(" " tcOMOFF "MPH" tcOTOG "KPH" tcOON ", ")); // 0-(full speed)

			outputTripFunctionValue(devLogOutput, dragDistanceIdx, tFuelEcon, nBuff, 0, dfOverflow9s); // trap mpg
			text::stringOut(devLogOutput, PSTR(" to "));

			doOutputJSONnumber(targetDistance, 2, PSTR(" " tcOMOFF "mi" tcOTOG "km" tcOON ", ")); // to [trap distance]

			break;

	}
#else // defined(useDragRaceFunction)
	text::stringOut(devLogOutput, PSTR("[this space intentionally left blank]"));
#endif // defined(useDragRaceFunction)

	JSON::openKey(devLogOutput, JSONrangesStr, JSONflagArray);
	// set scale at 40mpg or instant econ up to 999 mpg. Folks like to watch their mpg meter go to extremes
	JSON::outputNumber(devLogOutput, 18000ul, 3);
	JSON::outputNumber(devLogOutput, 24000ul, 3);
	JSON::outputNumber(devLogOutput, min(max(40000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 999000), 3);

	JSON::openKey(devLogOutput, JSONmeasuresStr, JSONflagArray);
	JSON::outputNumber(devLogOutput, currentIdx, tFuelEcon); // current fuel economy
	JSON::outputNumber(devLogOutput, tankIdx, tFuelEcon); // tank fuel economy

	JSON::openKey(devLogOutput, JSONmarkersStr, JSONflagArray);
	// instantaneous fuel economy, do not let scale exceed 999
	JSON::outputNumber(devLogOutput, min(999000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 3);

	JSON::closeElement(devLogOutput);

	JSON::closeElement(devLogOutput); // end JSON payload, and go trigger read on python.script

} // end sendJson function

#endif // defined(useJSONoutput)
