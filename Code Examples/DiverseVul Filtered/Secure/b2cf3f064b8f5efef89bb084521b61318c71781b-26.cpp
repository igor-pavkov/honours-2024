static int phar_file_type(HashTable *mimes, char *file, char **mime_type TSRMLS_DC) /* {{{ */
{
	char *ext;
	phar_mime_type *mime;
	ext = strrchr(file, '.');
	if (!ext) {
		*mime_type = "text/plain";
		/* no file extension = assume text/plain */
		return PHAR_MIME_OTHER;
	}
	++ext;
	if (SUCCESS != zend_hash_find(mimes, ext, strlen(ext), (void **) &mime)) {
		*mime_type = "application/octet-stream";
		return PHAR_MIME_OTHER;
	}
	*mime_type = mime->mime;
	return mime->type;
}