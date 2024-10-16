static krb5_error_code hdb_samba4_nextkey_panic(krb5_context context, HDB *db,
						unsigned flags,
						hdb_entry *entry)
{
	DBG_ERR("Attempt to iterate kpasswd keytab => PANIC\n");
	smb_panic("hdb_samba4_nextkey_panic: Attempt to iterate kpasswd keytab");
}