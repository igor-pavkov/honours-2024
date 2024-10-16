static const char *plug_to_string(test_plug_t plug)
{
	switch (plug) {
		case PLUG_NONE:
			return "open,   ";
		case PLUG_RESET:
			return "closed, ";
		case PLUG_TIMEOUT:
			return "timeout,";
		default:
			return "unknown,";
	}
}