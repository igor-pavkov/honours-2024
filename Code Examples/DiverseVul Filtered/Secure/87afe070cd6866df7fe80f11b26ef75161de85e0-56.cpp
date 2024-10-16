u32 parse_rap_ref(char *arg_val, u32 opt)
{
	if (arg_val) {
		if (sscanf(arg_val, "%d", &trackID) == 1) {
			parse_track_action(arg_val, opt ? TRAC_ACTION_REM_NON_REFS : TRAC_ACTION_REM_NON_RAP);
		}
	}
	hint_flags |= GP_RTP_PCK_SIGNAL_RAP;
	seg_at_rap = 1;
	return 0;
}