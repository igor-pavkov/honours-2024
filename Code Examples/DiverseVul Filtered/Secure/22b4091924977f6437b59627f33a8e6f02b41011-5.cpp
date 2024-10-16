static bool exact_match(bool has_wild,
			bool case_sensitive,
			const char *str,
			const char *mask)
{
	if (mask[0] == '.' && mask[1] == 0) {
		return false;
	}

	if (has_wild) {
		return false;
	}

	if (case_sensitive) {
		return strcmp(str,mask)==0;
	} else {
		return strcasecmp_m(str,mask) == 0;
	}
}