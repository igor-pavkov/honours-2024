uint32 CSoundFile::CalculateXParam(PATTERNINDEX pat, ROWINDEX row, CHANNELINDEX chn, bool *isExtended) const
{
	if(isExtended != nullptr) *isExtended = false;
	ROWINDEX maxCommands = 4;
	const ModCommand *m = Patterns[pat].GetpModCommand(row, chn);
	uint32 val = m->param;

	switch(m->command)
	{
	case CMD_OFFSET:
		// 24 bit command
		maxCommands = 2;
		break;
	case CMD_TEMPO:
	case CMD_PATTERNBREAK:
	case CMD_POSITIONJUMP:
		// 16 bit command
		maxCommands = 1;
		break;
	default:
		return val;
	}

	const bool xmTempoFix = m->command == CMD_TEMPO && GetType() == MOD_TYPE_XM;
	ROWINDEX numRows = std::min(Patterns[pat].GetNumRows() - row - 1, maxCommands);
	while(numRows > 0)
	{
		m += Patterns[pat].GetNumChannels();
		if(m->command != CMD_XPARAM)
		{
			break;
		}
		if(xmTempoFix && val < 256)
		{
			// With XM, 0x20 is the lowest tempo. Anything below changes ticks per row.
			val -= 0x20;
		}
		val = (val << 8) | m->param;
		numRows--;
		if(isExtended != nullptr) *isExtended = true;
	}
	return val;
}