u32 parse_dump_udta(char *code, u32 opt)
{
	char *sep;
	sep = strchr(code, ':');
	if (sep) {
		sep[0] = 0;
		dump_udta_track = atoi(code);
		sep[0] = ':';
		code = sep + 1;
	}

	if (strlen(code) == 4) {
		dump_udta_type = GF_4CC(code[0], code[1], code[2], code[3]);
	} else if (strlen(code) == 8) {
		// hex representation on 8 chars
		u32 hex1, hex2, hex3, hex4;
		if (sscanf(code, "%02x%02x%02x%02x", &hex1, &hex2, &hex3, &hex4) != 4) {
			M4_LOG(GF_LOG_ERROR, ("udta code is either a 4CC or 8 hex chars for non-printable 4CC\n"));
			return 2;
		}
		dump_udta_type = GF_4CC(hex1, hex2, hex3, hex4);
	} else {
		M4_LOG(GF_LOG_ERROR, ("udta code is either a 4CC or 8 hex chars for non-printable 4CC\n"));
		return 2;
	}
	return 0;
}