static char *get_input_encoding(void) {
	if (ICONVG(input_encoding) && ICONVG(input_encoding)[0]) {
		return ICONVG(input_encoding);
	} else if (PG(input_encoding) && PG(input_encoding)[0]) {
		return PG(input_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}