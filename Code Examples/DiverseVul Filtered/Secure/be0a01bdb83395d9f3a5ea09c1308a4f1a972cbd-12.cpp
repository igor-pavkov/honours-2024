static int get_file_info(
	/*! [in] Filename having the description document. */
	const char *filename,
	/*! [out] File information object having file attributes such as filelength,
	 * when was the file last modified, whether a file or a directory and
	 * whether the file or directory is readable. */
	OUT UpnpFileInfo *info)
{
	int code;
	struct stat s;
	FILE *fp;
	int rc = 0;
	time_t aux_LastModified;
	struct tm date;
	char buffer[ASCTIME_R_BUFFER_SIZE];

	UpnpFileInfo_set_ContentType(info, NULL);
	code = stat(filename, &s);
	if (code == -1)
		return -1;
	if (S_ISDIR(s.st_mode))
		UpnpFileInfo_set_IsDirectory(info, TRUE);
	else if (S_ISREG(s.st_mode))
		UpnpFileInfo_set_IsDirectory(info, FALSE);
	else
		return -1;
	/* check readable */
	fp = fopen(filename, "r");
	UpnpFileInfo_set_IsReadable(info, fp != NULL);
	if (fp)
		fclose(fp);
	UpnpFileInfo_set_FileLength(info, s.st_size);
	UpnpFileInfo_set_LastModified(info, s.st_mtime);
	rc = get_content_type(filename, info);
	aux_LastModified = UpnpFileInfo_get_LastModified(info);
	UpnpPrintf(UPNP_INFO, HTTP, __FILE__, __LINE__,
		"file info: %s, length: %lld, last_mod=%s readable=%d\n",
		filename,
		(long long)UpnpFileInfo_get_FileLength(info),
		web_server_asctime_r(http_gmtime_r(&aux_LastModified, &date), buffer),
		UpnpFileInfo_get_IsReadable(info));

	return rc;
}