static PHP_FUNCTION(preg_replace_callback)
{
	preg_replace_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1, 0);
}