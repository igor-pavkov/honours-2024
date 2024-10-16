char *sys_getwd(void)
{
#ifdef GETCWD_TAKES_NULL
	return getcwd(NULL, 0);
#elif HAVE_GETCWD
	char *wd = NULL, *s = NULL;
	size_t allocated = PATH_MAX;

	while (1) {
		s = SMB_REALLOC_ARRAY(s, char, allocated);
		if (s == NULL) {
			return NULL;
		}
		wd = getcwd(s, allocated);
		if (wd) {
			break;
		}
		if (errno != ERANGE) {
			SAFE_FREE(s);
			break;
		}
		allocated *= 2;
		if (allocated < PATH_MAX) {
			SAFE_FREE(s);
			break;
		}
	}
	return wd;
#else
	char *s = SMB_MALLOC_ARRAY(char, PATH_MAX);
	if (s == NULL) {
		return NULL;
	}
	return getwd(s);
#endif
}