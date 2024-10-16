static PHP_INI_MH(OnUpdateFlags)
{
	if (!new_value) {
		IF_G(default_filter_flags) = FILTER_FLAG_NO_ENCODE_QUOTES;
	} else {
		IF_G(default_filter_flags) = atoi(new_value);
	}
	return SUCCESS;
}