static krb5_error_code hdb_samba4_destroy(krb5_context context, HDB *db)
{
	talloc_free(db);
	return 0;
}