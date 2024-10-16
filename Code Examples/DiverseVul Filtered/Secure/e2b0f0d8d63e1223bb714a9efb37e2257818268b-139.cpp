static const char *attrval_to_str(uint8_t attr, uint8_t value)
{
	switch (attr) {
	case AVRCP_ATTRIBUTE_EQUALIZER:
		switch (value) {
		case AVRCP_EQUALIZER_ON:
			return "on";
		case AVRCP_EQUALIZER_OFF:
			return "off";
		}

		break;
	case AVRCP_ATTRIBUTE_REPEAT_MODE:
		switch (value) {
		case AVRCP_REPEAT_MODE_OFF:
			return "off";
		case AVRCP_REPEAT_MODE_SINGLE:
			return "singletrack";
		case AVRCP_REPEAT_MODE_ALL:
			return "alltracks";
		case AVRCP_REPEAT_MODE_GROUP:
			return "group";
		}

		break;
	/* Shuffle and scan have the same values */
	case AVRCP_ATTRIBUTE_SHUFFLE:
	case AVRCP_ATTRIBUTE_SCAN:
		switch (value) {
		case AVRCP_SCAN_OFF:
			return "off";
		case AVRCP_SCAN_ALL:
			return "alltracks";
		case AVRCP_SCAN_GROUP:
			return "group";
		}

		break;
	}

	return NULL;
}