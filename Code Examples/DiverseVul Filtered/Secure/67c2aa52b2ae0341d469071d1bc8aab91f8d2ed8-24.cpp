static INLINE BOOL FIELD_SKIP_BUFFER16(wStream* s, UINT32 TARGET_LEN)
{
	if (Stream_GetRemainingLength(s) < 2)
		return FALSE;

	Stream_Read_UINT16(s, TARGET_LEN);

	if (!Stream_SafeSeek(s, TARGET_LEN))
	{
		WLog_ERR(TAG, "error skipping %" PRIu32 " bytes", TARGET_LEN);
		return FALSE;
	}

	return TRUE;
}