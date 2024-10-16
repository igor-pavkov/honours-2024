static PHP_GSHUTDOWN_FUNCTION(pcre) /* {{{ */
{
	zend_hash_destroy(&pcre_globals->pcre_cache);
}