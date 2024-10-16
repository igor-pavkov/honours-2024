static __printf(1, 2) void verbose(const char *fmt, ...)
{
	va_list args;

	if (log_level == 0 || log_len >= log_size - 1)
		return;

	va_start(args, fmt);
	log_len += vscnprintf(log_buf + log_len, log_size - log_len, fmt, args);
	va_end(args);
}