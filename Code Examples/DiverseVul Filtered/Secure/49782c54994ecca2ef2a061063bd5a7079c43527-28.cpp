static void _close_ldap_link(zend_rsrc_list_entry *rsrc TSRMLS_DC) /* {{{ */
{
	ldap_linkdata *ld = (ldap_linkdata *)rsrc->ptr;

	ldap_unbind_ext(ld->link, NULL, NULL);
#if defined(LDAP_API_FEATURE_X_OPENLDAP) && defined(HAVE_3ARG_SETREBINDPROC)

	if (ld->rebindproc != NULL) {
		zval_dtor(ld->rebindproc);
		FREE_ZVAL(ld->rebindproc);
	}
#endif

	efree(ld);
	LDAPG(num_links)--;
}