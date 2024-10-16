static int play_status_to_val(const char *status)
{
	if (!strcasecmp(status, "stopped"))
		return AVRCP_PLAY_STATUS_STOPPED;
	else if (!strcasecmp(status, "playing"))
		return AVRCP_PLAY_STATUS_PLAYING;
	else if (!strcasecmp(status, "paused"))
		return AVRCP_PLAY_STATUS_PAUSED;
	else if (!strcasecmp(status, "forward-seek"))
		return AVRCP_PLAY_STATUS_FWD_SEEK;
	else if (!strcasecmp(status, "reverse-seek"))
		return AVRCP_PLAY_STATUS_REV_SEEK;
	else if (!strcasecmp(status, "error"))
		return AVRCP_PLAY_STATUS_ERROR;

	return -EINVAL;
}