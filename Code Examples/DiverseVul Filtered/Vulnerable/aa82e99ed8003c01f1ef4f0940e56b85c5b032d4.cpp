PHP_FUNCTION(locale_accept_from_http)
{
	UEnumeration *available;
	char *http_accept = NULL;
	int http_accept_len;
	UErrorCode status = 0;
	int len;
	char resultLocale[INTL_MAX_LOCALE_LEN+1];
	UAcceptResult outResult;

	if(zend_parse_parameters( ZEND_NUM_ARGS() TSRMLS_CC, "s", &http_accept, &http_accept_len) == FAILURE)
	{
		intl_error_set( NULL, U_ILLEGAL_ARGUMENT_ERROR,
		"locale_accept_from_http: unable to parse input parameters", 0 TSRMLS_CC );
		RETURN_FALSE;
	}

	available = ures_openAvailableLocales(NULL, &status);
	INTL_CHECK_STATUS(status, "locale_accept_from_http: failed to retrieve locale list");
	len = uloc_acceptLanguageFromHTTP(resultLocale, INTL_MAX_LOCALE_LEN,
						&outResult, http_accept, available, &status);
	uenum_close(available);
	INTL_CHECK_STATUS(status, "locale_accept_from_http: failed to find acceptable locale");
	if (len < 0 || outResult == ULOC_ACCEPT_FAILED) {
		RETURN_FALSE;
	}
	RETURN_STRINGL(resultLocale, len, 1);
}