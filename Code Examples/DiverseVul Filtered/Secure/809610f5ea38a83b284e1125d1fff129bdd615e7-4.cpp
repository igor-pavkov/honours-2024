static apr_status_t php_apache_child_shutdown(void *tmp)
{
	apache2_sapi_module.shutdown(&apache2_sapi_module);
#if defined(ZTS) && !defined(PHP_WIN32)
	tsrm_shutdown();
#endif
	return APR_SUCCESS;
}