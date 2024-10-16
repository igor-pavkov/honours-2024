int web_server_set_root_dir(const char *root_dir)
{
	size_t index;
	int ret;

	ret = membuffer_assign_str(&gDocumentRootDir, root_dir);
	if (ret != 0)
		return ret;
	/* remove trailing '/', if any */
	if (gDocumentRootDir.length > 0) {
		index = gDocumentRootDir.length - 1;	/* last char */
		if (gDocumentRootDir.buf[index] == '/')
			membuffer_delete(&gDocumentRootDir, index, 1);
	}

	return 0;
}