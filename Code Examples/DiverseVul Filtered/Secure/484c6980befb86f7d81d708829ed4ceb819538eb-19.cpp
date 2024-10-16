static krb5_error_code hdb_samba4_fetch_fast_cookie(krb5_context context,
						    struct samba_kdc_db_context *kdc_db_ctx,
						    hdb_entry *entry)
{
	krb5_error_code ret = SDB_ERR_NOENTRY;
	TALLOC_CTX *mem_ctx;
	struct ldb_result *res;
	int ldb_ret;
	struct sdb_entry sentry = {};
	const char *attrs[] = {
		"secret",
		NULL
	};
	const struct ldb_val *val;

	mem_ctx = talloc_named(kdc_db_ctx, 0, "samba_kdc_fetch context");
	if (!mem_ctx) {
		ret = ENOMEM;
		krb5_set_error_message(context, ret, "samba_kdc_fetch: talloc_named() failed!");
		return ret;
	}

	/* search for CN=FX-COOKIE */
	ldb_ret = ldb_search(kdc_db_ctx->secrets_db,
			     mem_ctx,
			     &res,
			     kdc_db_ctx->fx_cookie_dn,
			     LDB_SCOPE_BASE,
			     attrs, NULL);

	if (ldb_ret == LDB_ERR_NO_SUCH_OBJECT || res->count == 0) {

		ldb_ret = hdb_samba4_fill_fast_cookie(context,
						      kdc_db_ctx);

		if (ldb_ret != LDB_SUCCESS) {
			TALLOC_FREE(mem_ctx);
			return HDB_ERR_NO_WRITE_SUPPORT;
		}

		/* search for CN=FX-COOKIE */
		ldb_ret = ldb_search(kdc_db_ctx->secrets_db,
				     mem_ctx,
				     &res,
				     kdc_db_ctx->fx_cookie_dn,
				     LDB_SCOPE_BASE,
				     attrs, NULL);

		if (ldb_ret != LDB_SUCCESS || res->count != 1) {
			TALLOC_FREE(mem_ctx);
			return HDB_ERR_NOENTRY;
		}
	}

	val = ldb_msg_find_ldb_val(res->msgs[0],
				   "secret");
	if (val == NULL || val->length != 32) {
		TALLOC_FREE(mem_ctx);
		return HDB_ERR_NOENTRY;
	}


	ret = krb5_make_principal(context,
				  &sentry.principal,
				  KRB5_WELLKNOWN_ORG_H5L_REALM,
				  KRB5_WELLKNOWN_NAME, "org.h5l.fast-cookie",
				  NULL);
	if (ret) {
		TALLOC_FREE(mem_ctx);
		return ret;
	}

	ret = samba_kdc_set_fixed_keys(context, val, ENC_ALL_TYPES,
				       &sentry.keys);
	if (ret != 0) {
		return ret;
	}

	ret = sdb_entry_to_hdb_entry(context, &sentry, entry);
	sdb_entry_free(&sentry);
	TALLOC_FREE(mem_ctx);

	return ret;
}