BOOL utf8_string_to_rail_string(const char* string, RAIL_UNICODE_STRING* unicode_string)
{
	WCHAR* buffer = NULL;
	int length = 0;
	free(unicode_string->string);
	unicode_string->string = NULL;
	unicode_string->length = 0;

	if (!string || strlen(string) < 1)
		return TRUE;

	length = ConvertToUnicode(CP_UTF8, 0, string, -1, &buffer, 0);

	if ((length < 0) || ((size_t)length * sizeof(WCHAR) > UINT16_MAX))
	{
		free(buffer);
		return FALSE;
	}

	unicode_string->string = (BYTE*)buffer;
	unicode_string->length = (UINT16)length * sizeof(WCHAR);
	return TRUE;
}