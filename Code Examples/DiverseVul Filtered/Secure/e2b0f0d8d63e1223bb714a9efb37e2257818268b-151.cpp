static int attr_to_val(const char *str)
{
	if (!strcasecmp(str, "Equalizer"))
		return AVRCP_ATTRIBUTE_EQUALIZER;
	else if (!strcasecmp(str, "Repeat"))
		return AVRCP_ATTRIBUTE_REPEAT_MODE;
	else if (!strcasecmp(str, "Shuffle"))
		return AVRCP_ATTRIBUTE_SHUFFLE;
	else if (!strcasecmp(str, "Scan"))
		return AVRCP_ATTRIBUTE_SCAN;

	return -EINVAL;
}