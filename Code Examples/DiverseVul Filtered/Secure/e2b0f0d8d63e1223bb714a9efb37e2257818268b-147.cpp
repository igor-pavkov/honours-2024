static const char *status_to_string(uint8_t status)
{
	switch (status) {
	case AVRCP_PLAY_STATUS_STOPPED:
		return "stopped";
	case AVRCP_PLAY_STATUS_PLAYING:
		return "playing";
	case AVRCP_PLAY_STATUS_PAUSED:
		return "paused";
	case AVRCP_PLAY_STATUS_FWD_SEEK:
		return "forward-seek";
	case AVRCP_PLAY_STATUS_REV_SEEK:
		return "reverse-seek";
	case AVRCP_PLAY_STATUS_ERROR:
		return "error";
	default:
		return NULL;
	}
}