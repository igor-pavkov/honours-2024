static void hdb_samba4_free_entry_context(krb5_context context, struct HDB *db, hdb_entry *entry)
{
	/*
	 * This function is now called for every HDB entry, not just those with
	 * 'context' set, so we have to check that the context is not NULL.
	*/
	if (entry->context != NULL) {
		struct samba_kdc_entry *skdc_entry =
			talloc_get_type_abort(entry->context,
			struct samba_kdc_entry);

		/* this function is called only from hdb_free_entry().
		 * Make sure we neutralize the destructor or we will
		 * get a double free later when hdb_free_entry() will
		 * try to call free_hdb_entry() */
		entry->context = NULL;
		skdc_entry->kdc_entry = NULL;
		TALLOC_FREE(skdc_entry);
	}
}