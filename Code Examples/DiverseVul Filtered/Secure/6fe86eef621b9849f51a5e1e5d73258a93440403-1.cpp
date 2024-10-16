int CAPSTONE_API cs_winkernel_vsnprintf(char *buffer, size_t count, const char *format, va_list argptr)
{
	int result = _vsnprintf(buffer, count, format, argptr);

	// _vsnprintf() returns -1 when a string is truncated, and returns "count"
	// when an entire string is stored but without '\0' at the end of "buffer".
	// In both cases, null-terminator needs to be added manually.
	if (result == -1 || (size_t)result == count) {
		buffer[count - 1] = '\0';
	}

	if (result == -1) {
		// In case when -1 is returned, the function has to get and return a number
		// of characters that would have been written. This attempts so by retrying
		// the same conversion with temp buffer that is most likely big enough to
		// complete formatting and get a number of characters that would have been
		// written.
		char* tmp = cs_winkernel_malloc(0x1000);
		if (!tmp) {
			return result;
		}

		result = _vsnprintf(tmp, 0x1000, format, argptr);
		NT_ASSERT(result != -1);
		cs_winkernel_free(tmp);
	}

	return result;
}