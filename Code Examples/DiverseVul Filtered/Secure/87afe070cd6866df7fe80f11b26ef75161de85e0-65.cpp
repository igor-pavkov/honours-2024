u32 parse_sdp_ext(char *arg_val, u32 param)
{
	char *id;
	sdp_lines = gf_realloc(sdp_lines, sizeof(SDPLine) * (nb_sdp_ex + 1));
	if (!sdp_lines) return 2;
	id = strchr(arg_val, ':');
	if (id) {
		id[0] = 0;
		if (sscanf(arg_val, "%u", &sdp_lines[0].trackID) == 1) {
			id[0] = ':';
			sdp_lines[nb_sdp_ex].line = id + 1;
		}
		else {
			id[0] = ':';
			sdp_lines[nb_sdp_ex].line = arg_val;
			sdp_lines[nb_sdp_ex].trackID = 0;
		}
	}
	else {
		sdp_lines[nb_sdp_ex].line = arg_val;
		sdp_lines[nb_sdp_ex].trackID = 0;
	}
	open_edit = GF_TRUE;
	nb_sdp_ex++;
	return GF_FALSE;
}