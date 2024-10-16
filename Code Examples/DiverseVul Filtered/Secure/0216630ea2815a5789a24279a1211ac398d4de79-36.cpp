PHP_GSHUTDOWN_FUNCTION(openssl)
{
	if (openssl_globals->errors) {
		pefree(openssl_globals->errors, 1);
	}
}