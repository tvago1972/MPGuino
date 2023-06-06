#ifdef useDataLoggingOutput

static void doOutputDataLog(void)
{

	uint8_t tripIdx;
	uint8_t calcIdx;
	uint8_t c = ',';

	for (uint8_t x = 0; x < dLIcount; x++)
	{

		if ((x + 1) == dLIcount) c = '\n';

		tripIdx = pgm_read_byte(&dataLogInstr[(unsigned int)(x)][0]);
		calcIdx = pgm_read_byte(&dataLogInstr[(unsigned int)(x)][1]);

		text::numberOut(devLogOutput, tripIdx, calcIdx, mBuff1, 0, dfOverflow9s);
		text::stringOut(devLogOutput, thisCalcFuncObj.strBuffer);
		text::charOut(devLogOutput, c);

	}

}
#endif // useDataLoggingOutput
#ifdef useJSONoutput
static const uint8_t prgmJSON23tankFE[] PROGMEM = {
	instrMul2byByte, 2,									// multiply results by a 2
	instrDiv2byByte, 3,									// divide results by a 3
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmJSON53tankFE[] PROGMEM = {
	instrMul2byByte, 5,									// multiply results by a 5
	instrDiv2byByte, 3,									// divide results by a 3
	instrAdjustQuotient,								// bump up quotient by adjustment term (0 if remainder/divisor < 0.5, 1 if remainder/divisor >= 0.5)
	instrDone											// exit to caller
};

static const uint8_t prgmJSONminValue[] PROGMEM = {
	instrCmpXtoY, 0x21,
	instrBranchIfLTorE, 2,								// if register 2 contents are less than or equal to register 1 contents, skip
	instrLdReg, 0x12,									// load register 2 with contents of register 1

//ret:
	instrDone											// exit to caller
};

void doOutputJSONnumber(const uint8_t * sched, uint8_t tripIdx, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::runPrgm(sched, tripIdx);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(uint8_t tripIdx, uint8_t calcIdx, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::doCalculate(tripIdx, calcIdx);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(unsigned long an, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::init64((union union_64 *)(&s64reg[s64reg2]), an);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(union union_64 * an, uint8_t decimalPlaces, const char * labelStr)
{

	SWEET64::copy64((union union_64 *)(&s64reg[s64reg2]), an);
	doOutputJSONnumber(decimalPlaces, labelStr);

}

void doOutputJSONnumber(uint8_t decimalPlaces, const char * labelStr)
{

	text::stringOut(devLogOutput, ull2str(mBuff1, decimalPlaces, 0, dfOverflow9s)); // output the number part
	text::stringOut(devLogOutput, labelStr); // output the label part

}

void doOutputJSON(void) //skybolt added json output function
{

	static uint8_t subtitleCount1 = 2;
#ifdef useDragRaceFunction
	static uint8_t subtitleCount2 = 3;
#endif // useDragRaceFunction

	uint8_t oldSREG;

	uint64_t instFuelEcon;
	uint64_t tankFuelEcon;
	uint64_t tankFuelEcon53;

	if (timer0Status & t0sOutputJSON) // replaced timerChecker with this because systemInfo::cycles0() is a compile-time option that could break JSON output if #useCPUreading is not set
	{

		changeBitFlags(timer0Status, t0sOutputJSON, 0); // clear JSON subtitle change timer command

		if (!(--subtitleCount1)) subtitleCount1 = 2;
#ifdef useDragRaceFunction
		if (!(--subtitleCount2)) subtitleCount2 = 3;
#endif // useDragRaceFunction

	}

	unsigned long reserveQuantity = SWEET64::runPrgm(prgmReserveRemainingFuel, 0);

	unsigned long tankSize = SWEET64::runPrgm(prgmFetchParameterValue, pTankSizeIdx);

	text::stringOut(devLogOutput, PSTR("\n\n[")); // begin JSON payload

	// first graph, fuel
	text::stringOut(devLogOutput, PSTR("{\"title\":\""));

	if (reserveQuantity == 0) text::stringOut(devLogOutput, PSTR("fumes remaining, ")); // fumes left
	else doOutputJSONnumber(prgmReserveRemainingFuel, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " remaining, ")); // reserve remaining fuel left

	doOutputJSONnumber(prgmBingoFuelRemaining, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " e-reserve\",\n")); // bingo remaining fuel left
	//  doOutputJSONnumber(instantIdx, tEngineSpeed, 0, PSTR(" RPM\",\n")); // rpm to test latency only vs tachometer and LCD vs raspi indicator (expect 2x looptime)

	switch (subtitleCount1)
	{

		case 2:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"fuel used: "));

			doOutputJSONnumber(currentIdx, tFuelUsed, 3, PSTR(" trip " "\xEB" "gal" "\xEC" "L" "\xED" "; ")); // current trip fuel used
			doOutputJSONnumber(tankIdx, tFuelUsed, 3, PSTR(" tank " "\xEB" "gal" "\xEC" "L" "\xED" "; ")); // tank trip fuel used
			doOutputJSONnumber(instantIdx, tFuelRate, 3, PSTR(" using " "\xEB" "gal" "\xEC" "L" "\xED" "/hr\",\n")); // current rate of fuel burn in units/time

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"eco stats: "));
#ifdef trackIdleEOCdata
			doOutputJSONnumber(eocIdleTankIdx, tFuelUsed, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " used@idle, "));
			doOutputJSONnumber(eocIdleTankIdx, tDistance, 1, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " fuel cut\",\n"));
#else // trackIdleEOCdata
			if (reserveQuantity == 0) text::stringOut(devLogOutput, PSTR("fumes remaining, ")); // fumes left
			else doOutputJSONnumber(prgmReserveRemainingFuel, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " remaining, ")); // reserve remaining fuel left
			doOutputJSONnumber(prgmBingoFuelRemaining, 0, 3, PSTR(" " "\xEB" "gal" "\xEC" "L" "\xED" " e-reserve\",\n")); // bingo remaining fuel left
#endif // trackIdleEOCdata

			break;

	}

	reserveQuantity = SWEET64::runPrgm(prgmFetchParameterValue, pTankBingoSizeIdx);

	text::stringOut(devLogOutput, PSTR("\"ranges\":[")); //ranges do not have to be in order, d3js libraries will auto sort, so you can make it easier to read the code by changing the order
	doOutputJSONnumber(tankSize, 3, PSTR(",")); // largest, full tank size (e.g, 13.8 g)
	doOutputJSONnumber(tankSize - reserveQuantity, 3, PSTR(",")); // full tank less reserve (e.g.13.8g - 2.2g = 11.6g)
	doOutputJSONnumber(reserveQuantity, 3, PSTR("],\n")); // reserve amount (e.g. 2.2g)

	text::stringOut(devLogOutput, PSTR("\"measures\":["));
	doOutputJSONnumber(prgmReserveRemainingFuel, 0, 3, PSTR(",")); // reserve remaining fuel left
	doOutputJSONnumber(prgmRemainingFuel, 0, 3, PSTR("],\n")); // total remaining fuel left

	text::stringOut(devLogOutput, PSTR("\"markers\":["));
	doOutputJSONnumber(instantIdx, tFuelRate, 3, PSTR("]},\n\n")); // current rate of fuel burn in units/time

	// second graph, distance
	text::stringOut(devLogOutput, PSTR("{\"title\":\""));
	doOutputJSONnumber(prgmReserveDistanceToEmpty, tankIdx, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" " to e, ")); // distance to bingo
	doOutputJSONnumber(prgmBingoDistanceToEmpty, tankIdx, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " e-reserve\",\n")); // distance to fully empty tank from bingo

	switch (subtitleCount1)
	{

		case 2:
			//   text::stringOut(devLogOutput, PSTR("\"subtitle\":\"trip/tank distance: "));
			//   doOutputJSONnumber(currentIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" "/")); // current trip distance
			//   doOutputJSONnumber(tankIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" "\",\n")); // current trip distance

			text::stringOut(devLogOutput, PSTR("\"subtitle\":\""));
			doOutputJSONnumber(currentIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " trip distance, ")); // current trip distance
			doOutputJSONnumber(tankIdx, tDistance, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " tank distance\",\n")); // current trip distance

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\""));
			doOutputJSONnumber(prgmReserveDistance, tankIdx, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " safe range, ")); // reserve range
			doOutputJSONnumber(prgmRangeDistance, tankIdx, 2, PSTR(" " "\xEB" "mi" "\xEC" "km" "\xED" " dry range\",\n")); // distance to fully empty tank

			break;

	}

	text::stringOut(devLogOutput, PSTR("\"ranges\":["));
	doOutputJSONnumber(prgmRangeDistance, tankIdx, 1, PSTR(",")); // maximum range
	doOutputJSONnumber(prgmReserveDistance, tankIdx, 1, PSTR(",")); // range 2, safe range
	doOutputJSONnumber(prgmFindHalfReserveRange, tankIdx, 1, PSTR("],\n")); // range 3, half of safe range

	text::stringOut(devLogOutput, PSTR("\"measures\":["));
	doOutputJSONnumber(prgmBingoDistance, tankIdx, 2, PSTR("],\n")); // shows miles of e-reserve in bar form

	text::stringOut(devLogOutput, PSTR("\"markers\":["));
	doOutputJSONnumber(tankIdx, tDistanceToEmpty, 2, PSTR("]},\n\n")); // line is distance to empty

	// third graph, econ
	text::stringOut(devLogOutput, PSTR("{\"title\":\"")); //\xeb gal \xec L \xed/

#ifdef useDragRaceFunction
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
		text::stringOut(devLogOutput, PSTR(" ...\",\n"));

	}
	else // else not racing or waiting, go to normal
	{

		text::stringOut(devLogOutput, PSTR("trip/tank/inst: ")); //\xebgal\xecL\xed/
		doOutputJSONnumber(currentIdx, tFuelEcon, 3, PSTR("/ ")); // current fuel economy
		doOutputJSONnumber(tankIdx, tFuelEcon, 3, PSTR("/ ")); // tank fuel economy
		reserveQuantity = SWEET64::doCalculate(instantIdx, tFuelEcon);
		if (reserveQuantity > 9999999) text::stringOut(devLogOutput, PSTR("infinite\",\n"));
		else doOutputJSONnumber(instantIdx, tFuelEcon, 3, PSTR("\",\n")); // instantaneous fuel economy

	}

	switch (subtitleCount2)
	{

		case 3:
			//1 & 2 seconds display
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"accel time: 0-"));
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx) / 2, 0, PSTR("/"));
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx), 0, PSTR(": "));
			doOutputJSONnumber(dragHalfSpeedIdx, tAccelTestTime, 2, PSTR("/")); // 0-30 time
			doOutputJSONnumber(dragFullSpeedIdx, tAccelTestTime, 2, PSTR(", ")); // 0-60 time
			doOutputJSONnumber(dragDistanceIdx, tDistance * 1, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" " in ")); // trap distance
			doOutputJSONnumber(dragDistanceIdx, tAccelTestTime, 2, PSTR(" @")); // trap time
			doOutputJSONnumber(dragDistanceIdx, tSpeed, 3, PSTR("\xEB" "mph" "\xEC" "kph" "\xED" "; ")); // trap speed
			doOutputJSONnumber(dragHalfSpeedIdx, tEstimatedEnginePower, 0, PSTR("\xeB" "hp" "\xEC" "kW" "\xED" " @")); // estimated engine power
			doOutputJSONnumber(dragHalfSpeedIdx, tDragSpeed, 0, PSTR("\xEB" "mph" "\xEC" "kph" "\xED" "\",\n")); // max speed

			break;

		case 2:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"accel fuel: "));

			doOutputJSONnumber(dragHalfSpeedIdx, tFuelUsed, 3, PSTR("\xEB" "ga" "\xEC" "L" "\xED" " to ")); // 0-half fuel
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx) / 2, 0, PSTR(", ")); // 0-half speed
			doOutputJSONnumber(dragFullSpeedIdx, tFuelUsed, 3, PSTR("\xEB" "ga" "\xEC" "L" "\xED" " to ")); // 0-full fuel
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx), 0, PSTR(", ")); // 0-full speed
			doOutputJSONnumber(dragDistanceIdx, tFuelUsed, 3, PSTR("\xEB" "ga" "\xEC" "L" "\xED" " to ")); //trap fuel
			doOutputJSONnumber(dragDistanceIdx,  tDistance * 1, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" "\",\n")); // "to [trap distance]"  \xebft\xecm\xed;

			break;

		case 1:
			text::stringOut(devLogOutput, PSTR("\"subtitle\":\"accel fuel: "));

			doOutputJSONnumber(dragHalfSpeedIdx, tFuelEcon, 3, PSTR("\xEB" "mpg" "\xEC" "L100" "\xED" " to ")); // 0-30 mpg
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx) / 2, 0, PSTR(", ")); // 0-half speed
			doOutputJSONnumber(dragFullSpeedIdx, tFuelEcon, 3, PSTR("\xEB" "mpg" "\xEC" "L100" "\xED" " to ")); // 0-60 mpg
			doOutputJSONnumber(SWEET64::runPrgm(prgmFetchParameterValue, pDragSpeedIdx), 0, PSTR(", ")); // 0-60 speed
			doOutputJSONnumber(dragDistanceIdx, tFuelEcon, 3, PSTR("\xEB" "mpg" "\xEC" "L100" "\xED" " to ")); //trap mpg
			doOutputJSONnumber(dragDistanceIdx,  tDistance * 1, 2, PSTR("\xEB" "mi" "\xEC" "km" "\xED" "\",\n")); // "to [trap distance]"  \xebft\xecm\xed;

			break;

	}

#else // useDragRaceFunction
	text::stringOut(devLogOutput, PSTR("\"subtitle\":\"[this space intentionally left blank]\",\n"));

#endif // useDragRaceFunction
	text::stringOut(devLogOutput, PSTR("\"ranges\":[18,24,"));

	// set scale at 40mpg or instant econ up to 999 mpg. Folks like to watch their mpg meter go to extremes
	doOutputJSONnumber(min(max(40000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 999000), 3, PSTR("],\n"));

	text::stringOut(devLogOutput, PSTR("\"measures\":["));
	doOutputJSONnumber(currentIdx, tFuelEcon, 3, PSTR(",")); // current fuel economy
	doOutputJSONnumber(tankIdx, tFuelEcon, 3, PSTR("],\n")); // tank fuel economy

	text::stringOut(devLogOutput, PSTR("\"markers\":["));

	// instantaneous fuel economy, do not let scale exceed 999
	doOutputJSONnumber(min(999000, SWEET64::doCalculate(instantIdx, tFuelEcon)), 3, PSTR("]"));

	text::stringOut(devLogOutput, PSTR("\}]\n\r")); // end JSON payload, and go trigger read on python.script

} // end sendJson function

#endif // useJSONoutput
