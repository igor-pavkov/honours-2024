static int string_contains(const void *a, const void *b) {
	return !strstr((const char *)a, (const char *)b);
}