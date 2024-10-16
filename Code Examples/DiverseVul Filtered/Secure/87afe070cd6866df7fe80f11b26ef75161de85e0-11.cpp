u32 parse_track_action(char *arg, u32 act_type)
{
	if (!create_new_track_action(arg, act_type, 0)) {
		return 2;
	}
	return 0;
}