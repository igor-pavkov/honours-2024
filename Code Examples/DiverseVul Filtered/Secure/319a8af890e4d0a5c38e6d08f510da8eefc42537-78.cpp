static const char *error_type_string(gerbv_message_type_t type) {
	switch (type) {
	case GERBV_MESSAGE_FATAL:
		return _("FATAL");
	case GERBV_MESSAGE_ERROR:
		return _("ERROR");
	case GERBV_MESSAGE_WARNING:
		return _("Warning");
	case GERBV_MESSAGE_NOTE:
		return _("Note");
	default:
		return "Unknown";
	}
}