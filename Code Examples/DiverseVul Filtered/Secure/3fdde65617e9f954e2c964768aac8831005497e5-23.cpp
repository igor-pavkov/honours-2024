static int exif_process_undefined(char **result, char *value, size_t byte_count) {
	/* we cannot use strlcpy - here the problem is that we have to copy NUL
	 * chars up to byte_count, we also have to add a single NUL character to
	 * force end of string.
	 * estrndup does not return length
	 */
	if (byte_count) {
		(*result) = estrndup(value, byte_count); /* NULL @ byte_count!!! */
		return byte_count+1;
	}
	return 0;
}