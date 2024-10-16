static UPNP_INLINE int search_extension(
	/*! [in] . */
	const char *extension,
	/*! [out] . */
	const char **con_type,
	/*! [out] . */
	const char **con_subtype)
{
	int top, mid, bot;
	int cmp;

	top = 0;
	bot = NUM_MEDIA_TYPES - 1;

	while (top <= bot) {
		mid = (top + bot) / 2;
		cmp = strcasecmp(extension, gMediaTypeList[mid].file_ext);
		if (cmp > 0) {
			/* look below mid. */
			top = mid + 1;
		} else if (cmp < 0) {
			/* look above mid. */
			bot = mid - 1;
		} else {
			/* cmp == 0 */
			*con_type = gMediaTypeList[mid].content_type;
			*con_subtype = gMediaTypeList[mid].content_subtype;
			return 0;
		}
	}

	return -1;
}