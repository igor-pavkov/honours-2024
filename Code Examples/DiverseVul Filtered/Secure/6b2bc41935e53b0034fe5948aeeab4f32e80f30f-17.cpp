BOOL rail_read_unicode_string(wStream* s, RAIL_UNICODE_STRING* unicode_string)
{
	UINT16 new_len;
	BYTE* new_str;

	if (Stream_GetRemainingLength(s) < 2)
		return FALSE;

	Stream_Read_UINT16(s, new_len); /* cbString (2 bytes) */

	if (Stream_GetRemainingLength(s) < new_len)
		return FALSE;

	if (!new_len)
	{
		free(unicode_string->string);
		unicode_string->string = NULL;
		unicode_string->length = 0;
		return TRUE;
	}

	new_str = (BYTE*)realloc(unicode_string->string, new_len);

	if (!new_str)
	{
		free(unicode_string->string);
		unicode_string->string = NULL;
		return FALSE;
	}

	unicode_string->string = new_str;
	unicode_string->length = new_len;
	Stream_Read(s, unicode_string->string, unicode_string->length);
	return TRUE;
}