static bool make_krb5_skew_error(DATA_BLOB *pblob_out)
{
	krb5_context context = NULL;
	krb5_error_code kerr = 0;
	krb5_data reply;
	krb5_principal host_princ = NULL;
	char *host_princ_s = NULL;
	bool ret = False;

	*pblob_out = data_blob_null;

	initialize_krb5_error_table();
	kerr = krb5_init_context(&context);
	if (kerr) {
		return False;
	}
	/* Create server principal. */
	asprintf(&host_princ_s, "%s$@%s", global_myname(), lp_realm());
	if (!host_princ_s) {
		goto out;
	}
	strlower_m(host_princ_s);

	kerr = smb_krb5_parse_name(context, host_princ_s, &host_princ);
	if (kerr) {
		DEBUG(10,("make_krb5_skew_error: smb_krb5_parse_name failed "
			"for name %s: Error %s\n",
			host_princ_s, error_message(kerr) ));
		goto out;
	}

	kerr = smb_krb5_mk_error(context, KRB5KRB_AP_ERR_SKEW,
			host_princ, &reply);
	if (kerr) {
		DEBUG(10,("make_krb5_skew_error: smb_krb5_mk_error "
			"failed: Error %s\n",
			error_message(kerr) ));
		goto out;
	}

	*pblob_out = data_blob(reply.data, reply.length);
	kerberos_free_data_contents(context,&reply);
	ret = True;

  out:

	if (host_princ_s) {
		SAFE_FREE(host_princ_s);
	}
	if (host_princ) {
		krb5_free_principal(context, host_princ);
	}
	krb5_free_context(context);
	return ret;
}