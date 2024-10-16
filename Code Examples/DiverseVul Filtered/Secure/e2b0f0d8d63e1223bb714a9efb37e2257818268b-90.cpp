static const char *attr_to_str(uint8_t attr)
{
	switch (attr) {
	case AVRCP_ATTRIBUTE_EQUALIZER:
		return "Equalizer";
	case AVRCP_ATTRIBUTE_REPEAT_MODE:
		return "Repeat";
	case AVRCP_ATTRIBUTE_SHUFFLE:
		return "Shuffle";
	case AVRCP_ATTRIBUTE_SCAN:
		return "Scan";
	}

	return NULL;
}