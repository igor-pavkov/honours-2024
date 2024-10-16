static krb5_error_code hdb_samba4_kpasswd_fetch_kvno(krb5_context context, HDB *db,
						     krb5_const_principal _principal,
						     unsigned flags,
						     krb5_kvno _kvno,
						     hdb_entry *entry)
{
	struct samba_kdc_db_context *kdc_db_ctx = NULL;
	krb5_error_code ret;
	krb5_principal kpasswd_principal = NULL;

	kdc_db_ctx = talloc_get_type_abort(db->hdb_db,
					   struct samba_kdc_db_context);

	ret = smb_krb5_make_principal(context, &kpasswd_principal,
				      lpcfg_realm(kdc_db_ctx->lp_ctx),
				      "kadmin", "changepw",
				      NULL);
	if (ret) {
		return ret;
	}
	smb_krb5_principal_set_type(context, kpasswd_principal, KRB5_NT_SRV_INST);

	/*
	 * For the kpasswd service, always ensure we get the latest kvno. This
	 * also means we (correctly) refuse RODC-issued tickets.
	 */
	flags &= ~HDB_F_KVNO_SPECIFIED;

	/* Don't bother looking up a client or krbtgt. */
	flags &= ~(SDB_F_GET_CLIENT|SDB_F_GET_KRBTGT);

	ret = hdb_samba4_fetch_kvno(context, db,
				    kpasswd_principal,
				    flags,
				    0,
				    entry);

	krb5_free_principal(context, kpasswd_principal);
	return ret;
}