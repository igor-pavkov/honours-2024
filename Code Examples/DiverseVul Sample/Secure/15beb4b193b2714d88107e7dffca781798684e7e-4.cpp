static int server_gen_key_share(gnutls_session_t session, const gnutls_group_entry_st *group, gnutls_buffer_st *extdata)
{
	gnutls_datum_t tmp = {NULL, 0};
	int ret;

	if (group->pk != GNUTLS_PK_EC && group->pk != GNUTLS_PK_ECDH_X25519 &&
	    group->pk != GNUTLS_PK_ECDH_X448 &&
	    group->pk != GNUTLS_PK_DH) {
		_gnutls_debug_log("Cannot send key share for group %s!\n", group->name);
		return GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER;
	}

	_gnutls_handshake_log("EXT[%p]: sending key share for %s\n", session, group->name);

	ret =
	    _gnutls_buffer_append_prefix(extdata, 16, group->tls_id);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (group->pk == GNUTLS_PK_EC) {
		ret = _gnutls_ecc_ansi_x962_export(group->curve,
				session->key.kshare.ecdh_params.params[ECC_X],
				session->key.kshare.ecdh_params.params[ECC_Y],
				&tmp);
		if (ret < 0)
			return gnutls_assert_val(ret);

		ret =
		    _gnutls_buffer_append_data_prefix(extdata, 16, tmp.data, tmp.size);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		ret = 0;

	} else if (group->pk == GNUTLS_PK_ECDH_X25519 ||
		   group->pk == GNUTLS_PK_ECDH_X448) {
		ret =
		    _gnutls_buffer_append_data_prefix(extdata, 16,
				session->key.kshare.ecdhx_params.raw_pub.data,
				session->key.kshare.ecdhx_params.raw_pub.size);
		if (ret < 0)
			return gnutls_assert_val(ret);

		ret = 0;

	} else if (group->pk == GNUTLS_PK_DH) {
		ret =
		    _gnutls_buffer_append_prefix(extdata, 16, group->prime->size);
		if (ret < 0)
			return gnutls_assert_val(ret);

		ret = _gnutls_buffer_append_fixed_mpi(extdata, session->key.kshare.dh_params.params[DH_Y],
				group->prime->size);
		if (ret < 0)
			return gnutls_assert_val(ret);

		ret = 0;
	}

 cleanup:
	gnutls_free(tmp.data);
	return ret;
}