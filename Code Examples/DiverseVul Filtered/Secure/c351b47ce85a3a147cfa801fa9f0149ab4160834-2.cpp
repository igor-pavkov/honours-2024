static void pcre_handle_exec_error(int pcre_code TSRMLS_DC) /* {{{ */
{
	int preg_code = 0;

	switch (pcre_code) {
		case PCRE_ERROR_MATCHLIMIT:
			preg_code = PHP_PCRE_BACKTRACK_LIMIT_ERROR;
			break;

		case PCRE_ERROR_RECURSIONLIMIT:
			preg_code = PHP_PCRE_RECURSION_LIMIT_ERROR;
			break;

		case PCRE_ERROR_BADUTF8:
			preg_code = PHP_PCRE_BAD_UTF8_ERROR;
			break;

		case PCRE_ERROR_BADUTF8_OFFSET:
			preg_code = PHP_PCRE_BAD_UTF8_OFFSET_ERROR;
			break;

		default:
			preg_code = PHP_PCRE_INTERNAL_ERROR;
			break;
	}

	PCRE_G(error_code) = preg_code;
}