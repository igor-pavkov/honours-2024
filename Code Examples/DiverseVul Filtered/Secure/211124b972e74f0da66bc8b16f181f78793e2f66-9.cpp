static const char *dccp_reset_code(uint8_t code)
{
	if (code >= __DCCP_RESET_CODE_LAST)
		return "invalid";
	return dccp_reset_codes[code];
}