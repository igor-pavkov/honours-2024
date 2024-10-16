static char *get_output_encoding(void) {
	if (ICONVG(output_encoding) && ICONVG(output_encoding)[0]) {
		return ICONVG(output_encoding);
	} else if (PG(output_encoding) && PG(output_encoding)[0]) {
		return PG(output_encoding);
	} else if (SG(default_charset)) {
		return SG(default_charset);
	}
	return "";
}