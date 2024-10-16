static krb5_error_code hdb_samba4_store(krb5_context context, HDB *db, unsigned flags, hdb_entry *entry)
{
	return HDB_ERR_DB_INUSE;
}