static UPNP_INLINE void media_list_init(void)
{
	int i;
	const char *s = gEncodedMediaTypes;
	struct document_type_t *doc_type;

	for (i = 0; *s != '\0'; i++) {
		doc_type = &gMediaTypeList[i];
		doc_type->file_ext = s;
		/* point to type. */
		s += strlen(s) + 1;
		doc_type->content_type = gMediaTypes[(int)*s];
		/* point to subtype. */
		s++;
		doc_type->content_subtype = s;
		/* next entry. */
		s += strlen(s) + 1;
	}
	assert(i == NUM_MEDIA_TYPES);
}