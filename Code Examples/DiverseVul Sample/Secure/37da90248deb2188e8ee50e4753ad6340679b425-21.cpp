ZEND_API int zend_startup_strtod(void) /* {{{ */
{
#ifdef ZTS
	dtoa_mutex = tsrm_mutex_alloc();
	pow5mult_mutex = tsrm_mutex_alloc();
#endif
	return 1;
}