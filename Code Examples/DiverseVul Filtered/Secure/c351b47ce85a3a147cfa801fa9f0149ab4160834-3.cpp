static PHP_FUNCTION(preg_match_all)
{
	php_do_pcre_match(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}