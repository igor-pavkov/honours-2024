void proto_report_dissector_bug(const char *format, ...)
{
	va_list args;

	if (wireshark_abort_on_dissector_bug) {
		/*
		 * Try to have the error message show up in the crash
		 * information.
		 */
		va_start(args, format);
		ws_vadd_crash_info(format, args);
		va_end(args);

		/*
		 * Print the error message.
		 */
		va_start(args, format);
		vfprintf(stderr, format, args);
		va_end(args);
		putc('\n', stderr);

		/*
		 * And crash.
		 */
		abort();
	} else {
		va_start(args, format);
		VTHROW_FORMATTED(DissectorError, format, args);
		va_end(args);
	}
}