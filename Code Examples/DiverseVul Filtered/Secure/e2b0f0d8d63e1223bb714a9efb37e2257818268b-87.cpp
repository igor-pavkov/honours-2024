static const char *battery_status_to_str(uint8_t status)
{
	switch (status) {
	case AVRCP_BATTERY_STATUS_NORMAL:
		return "normal";
	case AVRCP_BATTERY_STATUS_WARNING:
		return "warning";
	case AVRCP_BATTERY_STATUS_CRITICAL:
		return "critical";
	case AVRCP_BATTERY_STATUS_EXTERNAL:
		return "external";
	case AVRCP_BATTERY_STATUS_FULL_CHARGE:
		return "fullcharge";
	}

	return NULL;
}