static void _free_ldap_result(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	LDAPMessage *result = (LDAPMessage *)rsrc->ptr;
	ldap_msgfree(result);
}