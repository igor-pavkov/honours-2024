static void php_snmp_object_free_storage(zend_object *object) /* {{{ */
{
	php_snmp_object *intern = php_snmp_fetch_object(object);

	if (!intern) {
		return;
	}

	netsnmp_session_free(&(intern->session));

	zend_object_std_dtor(&intern->zo);
}