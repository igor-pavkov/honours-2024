u32 parse_cp_loc(char *arg_val, u32 opt)
{
	if (!strcmp(arg_val, "both")) cp_location_mode = GF_DASH_CPMODE_BOTH;
	else if (!strcmp(arg_val, "as")) cp_location_mode = GF_DASH_CPMODE_ADAPTATION_SET;
	else if (!strcmp(arg_val, "rep")) cp_location_mode = GF_DASH_CPMODE_REPRESENTATION;
	else {
		M4_LOG(GF_LOG_ERROR, ("Unrecognized ContentProtection loction mode \"%s\" - please check usage\n", arg_val));
		return 2;
	}
	return 0;
}