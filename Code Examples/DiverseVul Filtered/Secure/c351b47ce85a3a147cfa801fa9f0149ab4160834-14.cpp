static PHP_GINIT_FUNCTION(pcre) /* {{{ */
{
	zend_hash_init(&pcre_globals->pcre_cache, 0, NULL, php_free_pcre_cache, 1);
	pcre_globals->backtrack_limit = 0;
	pcre_globals->recursion_limit = 0;
	pcre_globals->error_code      = PHP_PCRE_NO_ERROR;
}