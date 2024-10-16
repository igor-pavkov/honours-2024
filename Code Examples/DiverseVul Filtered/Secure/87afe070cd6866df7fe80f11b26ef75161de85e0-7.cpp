u32 parse_pssh(char *arg_val, u32 opt)
{
	if (!strcmp(arg_val, "f")) pssh_mode = GF_DASH_PSSH_MOOF;
	else if (!strcmp(arg_val, "v")) pssh_mode = GF_DASH_PSSH_MOOV;
	else if (!strcmp(arg_val, "m")) pssh_mode = GF_DASH_PSSH_MPD;
	else if (!strcmp(arg_val, "mf") || !strcmp(arg_val, "fm")) pssh_mode = GF_DASH_PSSH_MOOF_MPD;
	else if (!strcmp(arg_val, "mv") || !strcmp(arg_val, "vm")) pssh_mode = GF_DASH_PSSH_MOOV_MPD;
	else pssh_mode = GF_DASH_PSSH_MOOV;
	return 0;
}