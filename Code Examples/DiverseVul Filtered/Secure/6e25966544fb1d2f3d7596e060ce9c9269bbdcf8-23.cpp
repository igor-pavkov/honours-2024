static void free_php_snmp_properties(zval *el)  /* {{{ */
{
	pefree(Z_PTR_P(el), 1);
}