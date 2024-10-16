static void php_set_opts(LDAP *ldap, int sizelimit, int timelimit, int deref, int *old_sizelimit, int *old_timelimit, int *old_deref)
{
	/* sizelimit */
	if (sizelimit > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_NSLDAP || HAVE_ORALDAP
		ldap_get_option(ldap, LDAP_OPT_SIZELIMIT, old_sizelimit);
		ldap_set_option(ldap, LDAP_OPT_SIZELIMIT, &sizelimit);
#else
		*old_sizelimit = ldap->ld_sizelimit;
		ldap->ld_sizelimit = sizelimit;
#endif
	}

	/* timelimit */
	if (timelimit > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_NSLDAP || HAVE_ORALDAP
		ldap_get_option(ldap, LDAP_OPT_TIMELIMIT, old_timelimit);
		ldap_set_option(ldap, LDAP_OPT_TIMELIMIT, &timelimit);
#else
		*old_timelimit = ldap->ld_timelimit;
		ldap->ld_timelimit = timelimit;
#endif
	}

	/* deref */
	if (deref > -1) {
#if (LDAP_API_VERSION >= 2004) || HAVE_NSLDAP || HAVE_ORALDAP
		ldap_get_option(ldap, LDAP_OPT_DEREF, old_deref);
		ldap_set_option(ldap, LDAP_OPT_DEREF, &deref);
#else
		*old_deref = ldap->ld_deref;
		ldap->ld_deref = deref;
#endif
	}
}