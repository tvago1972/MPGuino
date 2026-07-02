#if defined(useButtonInput)
/* basic top-down menu section */

static uint8_t baseMenu::menuHandler(uint8_t cmd, uint8_t cursorPos)
{

	uint8_t retVal = 0;

	switch (cmd)
	{

		case menuFirstLineOutIdx:
			text::stringOut(m8DevLCDidx, baseMenuTitles, cursorPos);
			break;

		case menuDoSelectionIdx:
			switch (cursorPos)
			{

				case 0:
#if defined(useExpandedMainDisplay)
					retVal = mainMenuDisplayIdx;
#else // defined(useExpandedMainDisplay)
					retVal = mainDisplayIdx;
#endif // defined(useExpandedMainDisplay)

					break;

				case 1:
					retVal = settingsMenuDisplayIdx;
					break;

				default:
					retVal = optionalDisplayIdxStart + cursorPos - 2;
					break;

			}
			break;

		case menuExitIdx:
#if defined(useExpandedMainDisplay)
			retVal = displayCursor[(uint16_t)(mainMenuDisplayIdx)] + mainDisplayIdx;
#else // defined(useExpandedMainDisplay)
			retVal = mainDisplayIdx;
#endif // defined(useExpandedMainDisplay)
			break;

		default:
			break;

	}

	return retVal;

}

#endif // defined(useButtonInput)
