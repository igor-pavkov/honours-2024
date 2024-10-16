static UPNP_INLINE int get_alias(
	/*! [in] request file passed in to be compared with. */
	const char *request_file,
	/*! [out] xml alias object which has a file name stored. */
	struct xml_alias_t *alias,
	/*! [out] File information object which will be filled up if the file
	 * comparison succeeds. */
	UpnpFileInfo *info)
{
	int cmp = strcmp(alias->name.buf, request_file);
	if (cmp == 0) {
		UpnpFileInfo_set_FileLength(info, (off_t)alias->doc.length);
		UpnpFileInfo_set_IsDirectory(info, FALSE);
		UpnpFileInfo_set_IsReadable(info, TRUE);
		UpnpFileInfo_set_LastModified(info, alias->last_modified);
	}

	return cmp == 0;
}