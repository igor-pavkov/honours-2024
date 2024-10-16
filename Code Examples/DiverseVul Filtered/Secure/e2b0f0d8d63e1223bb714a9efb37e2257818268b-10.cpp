static const char *status_to_str(uint8_t status)
{
	switch (status) {
	case AVRCP_STATUS_INVALID_COMMAND:
		return "Invalid Command";
	case AVRCP_STATUS_INVALID_PARAM:
		return "Invalid Parameter";
	case AVRCP_STATUS_INTERNAL_ERROR:
		return "Internal Error";
	case AVRCP_STATUS_SUCCESS:
		return "Success";
	default:
		return "Unknown";
	}
}