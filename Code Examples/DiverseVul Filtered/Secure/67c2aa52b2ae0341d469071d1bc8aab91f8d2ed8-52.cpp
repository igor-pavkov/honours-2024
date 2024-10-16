static BOOL update_read_field_flags(wStream* s, UINT32* fieldFlags, BYTE flags, BYTE fieldBytes)
{
	int i;
	BYTE byte;

	if (flags & ORDER_ZERO_FIELD_BYTE_BIT0)
		fieldBytes--;

	if (flags & ORDER_ZERO_FIELD_BYTE_BIT1)
	{
		if (fieldBytes > 1)
			fieldBytes -= 2;
		else
			fieldBytes = 0;
	}

	if (Stream_GetRemainingLength(s) < fieldBytes)
		return FALSE;

	*fieldFlags = 0;

	for (i = 0; i < fieldBytes; i++)
	{
		Stream_Read_UINT8(s, byte);
		*fieldFlags |= byte << (i * 8);
	}

	return TRUE;
}