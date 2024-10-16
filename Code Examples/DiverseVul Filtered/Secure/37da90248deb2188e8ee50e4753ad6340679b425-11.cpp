ZEND_API int zend_shutdown_strtod(void) /* {{{ */
{
	destroy_freelist();
#ifdef ZTS
	tsrm_mutex_free(dtoa_mutex);
	dtoa_mutex = NULL;

	tsrm_mutex_free(pow5mult_mutex);
	pow5mult_mutex = NULL;
#endif
	return 1;
}