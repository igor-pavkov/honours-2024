PHP_FUNCTION(snmp_set_quick_print)
{
	zend_long a1;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &a1) == FAILURE) {
		RETURN_FALSE;
	}

	netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT, (int)a1);
	RETURN_TRUE;
}