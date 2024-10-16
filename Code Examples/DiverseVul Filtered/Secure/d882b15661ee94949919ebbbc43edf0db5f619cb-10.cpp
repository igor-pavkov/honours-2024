static void debug_handler(int level, const char *format, va_list ap)
{
	if (verbose) {
		vprintf(format, ap);
	} else {
		char buf[4096], *tmp;
		int len;
	
		if (vsnprintf(buf, sizeof(buf), format, ap) >= sizeof(buf)) {
			fprintf(stderr, "Increase temporary log buffer size!\n");
			return;
		}

		if (pthread_mutex_lock(&log_mutex) != 0) {
			fprintf(stderr, "pthread_mutex_lock failed!\n");
			return;
		}

		len = (log_buffer != NULL) ? strlen(log_buffer) : 0;

		tmp = realloc(log_buffer, len + strlen(buf) + 1);

		if (tmp != NULL) {
			log_buffer = tmp;
			strcpy(log_buffer + len, buf);
		} else {
			fprintf(stderr, "Out of memory for log buffer!\n");
		}

		if (pthread_mutex_unlock(&log_mutex) != 0) {
			fprintf(stderr, "pthread_mutex_unlock failed!\n");
			failure();
		}
	}
}