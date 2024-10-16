static int attrval_to_val(uint8_t attr, const char *value)
{
	int ret;

	switch (attr) {
	case AVRCP_ATTRIBUTE_EQUALIZER:
		if (!strcmp(value, "off"))
			ret = AVRCP_EQUALIZER_OFF;
		else if (!strcmp(value, "on"))
			ret = AVRCP_EQUALIZER_ON;
		else
			ret = -EINVAL;

		return ret;
	case AVRCP_ATTRIBUTE_REPEAT_MODE:
		if (!strcmp(value, "off"))
			ret = AVRCP_REPEAT_MODE_OFF;
		else if (!strcmp(value, "singletrack"))
			ret = AVRCP_REPEAT_MODE_SINGLE;
		else if (!strcmp(value, "alltracks"))
			ret = AVRCP_REPEAT_MODE_ALL;
		else if (!strcmp(value, "group"))
			ret = AVRCP_REPEAT_MODE_GROUP;
		else
			ret = -EINVAL;

		return ret;
	case AVRCP_ATTRIBUTE_SHUFFLE:
		if (!strcmp(value, "off"))
			ret = AVRCP_SHUFFLE_OFF;
		else if (!strcmp(value, "alltracks"))
			ret = AVRCP_SHUFFLE_ALL;
		else if (!strcmp(value, "group"))
			ret = AVRCP_SHUFFLE_GROUP;
		else
			ret = -EINVAL;

		return ret;
	case AVRCP_ATTRIBUTE_SCAN:
		if (!strcmp(value, "off"))
			ret = AVRCP_SCAN_OFF;
		else if (!strcmp(value, "alltracks"))
			ret = AVRCP_SCAN_ALL;
		else if (!strcmp(value, "group"))
			ret = AVRCP_SCAN_GROUP;
		else
			ret = -EINVAL;

		return ret;
	}

	return -EINVAL;
}