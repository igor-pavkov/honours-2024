static UPNP_INLINE int get_content_type(
	/*! [in] . */
	const char *filename,
	/*! [out] . */
	OUT UpnpFileInfo *fileInfo)
{
	const char *extension;
	const char *type;
	const char *subtype;
	int ctype_found = FALSE;
	char *temp = NULL;
	size_t length = 0;
	int rc = 0;

	UpnpFileInfo_set_ContentType(fileInfo, NULL);
	/* get ext */
	extension = strrchr(filename, '.');
	if (extension != NULL)
		if (search_extension(extension + 1, &type, &subtype) == 0)
			ctype_found = TRUE;
	if (!ctype_found) {
		/* unknown content type */
		type = gMediaTypes[APPLICATION_INDEX];
		subtype = "octet-stream";
	}
	length = strlen(type) + strlen("/") + strlen(subtype) + 1;
	temp = malloc(length);
	if (!temp)
		return UPNP_E_OUTOF_MEMORY;
	rc = snprintf(temp, length, "%s/%s", type, subtype);
	if (rc < 0 || (unsigned int) rc >= length) {
		free(temp);
		return UPNP_E_OUTOF_MEMORY;
	}
	UpnpFileInfo_set_ContentType(fileInfo, temp);
	free(temp);
	if (!UpnpFileInfo_get_ContentType(fileInfo))
		return UPNP_E_OUTOF_MEMORY;

	return 0;
}