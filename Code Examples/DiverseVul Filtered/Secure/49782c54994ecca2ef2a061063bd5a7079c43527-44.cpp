static void _free_ldap_result_entry(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	ldap_resultentry *entry = (ldap_resultentry *)rsrc->ptr;

	if (entry->ber != NULL) {
		ber_free(entry->ber, 0);
		entry->ber = NULL;
	}
	zend_list_delete(entry->id);
	efree(entry);
}