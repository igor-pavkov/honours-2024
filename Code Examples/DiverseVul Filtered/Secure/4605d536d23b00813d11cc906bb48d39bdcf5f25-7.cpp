static int unlink_filename(char **filename TSRMLS_DC) /* {{{ */
{
	VCWD_UNLINK(*filename);
	return 0;
}