static int authorizer(void *autharg, int access_type, const char *arg3, const char *arg4,
		const char *arg5, const char *arg6)
{
	char *filename;
	switch (access_type) {
		case SQLITE_COPY: {
			TSRMLS_FETCH();
			filename = make_filename_safe(arg4 TSRMLS_CC);
			if (!filename) {
				return SQLITE_DENY;
			}
			efree(filename);
			return SQLITE_OK;
		}

		case SQLITE_ATTACH: {
			TSRMLS_FETCH();
			filename = make_filename_safe(arg3 TSRMLS_CC);
			if (!filename) {
				return SQLITE_DENY;
			}
			efree(filename);
			return SQLITE_OK;
		}

		default:
			/* access allowed */
			return SQLITE_OK;
	}
}