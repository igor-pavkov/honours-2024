PHP_FUNCTION(snmp_get_valueretrieval)
{
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(SNMP_G(valueretrieval));
}