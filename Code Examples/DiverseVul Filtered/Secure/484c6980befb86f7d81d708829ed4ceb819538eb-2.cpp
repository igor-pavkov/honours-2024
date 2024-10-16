static krb5_error_code hdb_samba4_fetch_kvno(krb5_context context, HDB *db,
					     krb5_const_principal principal,
					     unsigned flags,
					     krb5_kvno kvno,
					     hdb_entry *entry)
{
	struct samba_kdc_db_context *kdc_db_ctx;
	struct sdb_entry sentry = {};
	krb5_error_code code, ret;
	uint32_t sflags;

	kdc_db_ctx = talloc_get_type_abort(db->hdb_db,
					   struct samba_kdc_db_context);

	if (flags & HDB_F_GET_FAST_COOKIE) {
		return hdb_samba4_fetch_fast_cookie(context,
						    kdc_db_ctx,
						    entry);
	}

	sflags = (flags & SDB_F_HDB_MASK);

	ret = samba_kdc_fetch(context,
			      kdc_db_ctx,
			      principal,
			      sflags,
			      kvno,
			      &sentry);
	switch (ret) {
	case 0:
		code = 0;
		break;
	case SDB_ERR_WRONG_REALM:
		/*
		 * If SDB_ERR_WRONG_REALM is returned we need to process the
		 * sdb_entry to fill the principal in the HDB entry.
		 */
		code = HDB_ERR_WRONG_REALM;
		break;
	case SDB_ERR_NOENTRY:
		return HDB_ERR_NOENTRY;
	case SDB_ERR_NOT_FOUND_HERE:
		return HDB_ERR_NOT_FOUND_HERE;
	default:
		return ret;
	}

	ret = sdb_entry_to_hdb_entry(context, &sentry, entry);
	sdb_entry_free(&sentry);

	if (code != 0 && ret != 0) {
		code = ret;
	}

	return code;
}