u32 parse_track_dump(char *arg, u32 dump_type)
{
	if (!create_new_track_action(arg, TRAC_ACTION_RAW_EXTRACT, dump_type))
		return 2;
	track_dump_type = dump_type;
	return 0;
}