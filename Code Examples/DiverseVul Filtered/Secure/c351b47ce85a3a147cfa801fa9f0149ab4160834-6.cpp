static PHP_FUNCTION(preg_match)
{
	php_do_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}