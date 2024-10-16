PHP_FUNCTION(link)
{
	char *topath, *frompath;
	size_t topath_len, frompath_len;
	int ret;
	char source_p[MAXPATHLEN];
	char dest_p[MAXPATHLEN];

	/*First argument to link function is the target and hence should go to frompath
	  Second argument to link function is the link itself and hence should go to topath */
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &frompath, &frompath_len, &topath, &topath_len) == FAILURE) {
		return;
	}

	if (!expand_filepath(frompath, source_p) || !expand_filepath(topath, dest_p)) {
		php_error_docref(NULL, E_WARNING, "No such file or directory");
		RETURN_FALSE;
	}

	if (php_stream_locate_url_wrapper(source_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY) ||
		php_stream_locate_url_wrapper(dest_p, NULL, STREAM_LOCATE_WRAPPERS_ONLY) )
	{
		php_error_docref(NULL, E_WARNING, "Unable to link to a URL");
		RETURN_FALSE;
	}

	if (OPENBASEDIR_CHECKPATH(source_p)) {
		RETURN_FALSE;
	}

	if (OPENBASEDIR_CHECKPATH(dest_p)) {
		RETURN_FALSE;
	}

#ifndef ZTS
	ret = CreateHardLinkA(topath, frompath, NULL);
#else
	ret = CreateHardLinkA(dest_p, source_p, NULL);
#endif

	if (ret == 0) {
		php_error_docref(NULL, E_WARNING, "%s", strerror(errno));
		RETURN_FALSE;
	}

	RETURN_TRUE;
}