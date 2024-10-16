PHP_METHOD(snmp, __construct)
{
	php_snmp_object *snmp_object;
	zval *object = getThis();
	char *a1, *a2;
	size_t a1_len, a2_len;
	zend_long timeout = SNMP_DEFAULT_TIMEOUT;
	zend_long retries = SNMP_DEFAULT_RETRIES;
	zend_long version = SNMP_DEFAULT_VERSION;
	int argc = ZEND_NUM_ARGS();

	snmp_object = Z_SNMP_P(object);

	if (zend_parse_parameters_throw(argc, "lss|ll", &version, &a1, &a1_len, &a2, &a2_len, &timeout, &retries) == FAILURE) {
		return;
	}

	switch (version) {
		case SNMP_VERSION_1:
		case SNMP_VERSION_2c:
		case SNMP_VERSION_3:
			break;
		default:
			zend_throw_exception(zend_ce_exception, "Unknown SNMP protocol version", 0);
			return;
	}

	/* handle re-open of snmp session */
	if (snmp_object->session) {
		netsnmp_session_free(&(snmp_object->session));
	}

	if (netsnmp_session_init(&(snmp_object->session), version, a1, a2, timeout, retries)) {
		return;
	}
	snmp_object->max_oids = 0;
	snmp_object->valueretrieval = SNMP_G(valueretrieval);
	snmp_object->enum_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_PRINT_NUMERIC_ENUM);
	snmp_object->oid_output_format = netsnmp_ds_get_int(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_OID_OUTPUT_FORMAT);
	snmp_object->quick_print = netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID, NETSNMP_DS_LIB_QUICK_PRINT);
	snmp_object->oid_increasing_check = TRUE;
	snmp_object->exceptions_enabled = 0;
}