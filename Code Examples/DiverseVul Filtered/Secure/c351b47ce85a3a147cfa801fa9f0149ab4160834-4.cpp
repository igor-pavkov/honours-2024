static PHP_FUNCTION(preg_filter)
{
	preg_replace_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0, 1);
}