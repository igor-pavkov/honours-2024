static krb5_error_code hdb_samba4_nextkey(krb5_context context, HDB *db, unsigned flags,
				   hdb_entry *entry)
{
	struct samba_kdc_db_context *kdc_db_ctx;
	struct sdb_entry sentry = {};
	krb5_error_code ret;

	kdc_db_ctx = talloc_get_type_abort(db->hdb_db,
					   struct samba_kdc_db_context);

	ret = samba_kdc_nextkey(context, kdc_db_ctx, &sentry);
	switch (ret) {
	case 0:
		break;
	case SDB_ERR_WRONG_REALM:
		return HDB_ERR_WRONG_REALM;
	case SDB_ERR_NOENTRY:
		return HDB_ERR_NOENTRY;
	case SDB_ERR_NOT_FOUND_HERE:
		return HDB_ERR_NOT_FOUND_HERE;
	default:
		return ret;
	}

	ret = sdb_entry_to_hdb_entry(context, &sentry, entry);
	sdb_entry_free(&sentry);
	return ret;
}