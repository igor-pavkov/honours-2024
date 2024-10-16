NTSTATUS hdb_samba4_kpasswd_create_kdc(struct samba_kdc_base_context *base_ctx,
				       krb5_context context, struct HDB **db)
{
	NTSTATUS nt_status;

	nt_status = hdb_samba4_create_kdc(base_ctx, context, db);
	if (!NT_STATUS_IS_OK(nt_status)) {
		return nt_status;
	}

	(*db)->hdb_fetch_kvno = hdb_samba4_kpasswd_fetch_kvno;
	(*db)->hdb_firstkey = hdb_samba4_nextkey_panic;
	(*db)->hdb_nextkey = hdb_samba4_nextkey_panic;

	return NT_STATUS_OK;
}