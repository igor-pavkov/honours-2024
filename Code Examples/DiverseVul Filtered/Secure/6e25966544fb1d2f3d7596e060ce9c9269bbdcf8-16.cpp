static zend_object *php_snmp_object_new(zend_class_entry *class_type) /* {{{ */
{
	php_snmp_object *intern;

	/* Allocate memory for it */
	intern = ecalloc(1, sizeof(php_snmp_object) + zend_object_properties_size(class_type));

	zend_object_std_init(&intern->zo, class_type);
	object_properties_init(&intern->zo, class_type);

	intern->zo.handlers = &php_snmp_object_handlers;

	return &intern->zo;

}