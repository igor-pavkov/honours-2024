static int client_gen_key_share(gnutls_session_t session, const gnutls_group_entry_st *group, gnutls_buffer_st *extdata)
{
	gnutls_datum_t tmp = {NULL, 0};
	int ret;

	if (group->pk != GNUTLS_PK_EC && group->pk != GNUTLS_PK_ECDH_X25519 &&
	    group->pk != GNUTLS_PK_ECDH_X448 &&
	    group->pk != GNUTLS_PK_DH) {
		_gnutls_debug_log("Cannot send key share for group %s!\n", group->name);
		return GNUTLS_E_INT_RET_0;
	}

	_gnutls_handshake_log("EXT[%p]: sending key share for %s\n", session, group->name);

	ret =
	    _gnutls_buffer_append_prefix(extdata, 16, group->tls_id);
	if (ret < 0)
		return gnutls_assert_val(ret);

	if (group->pk == GNUTLS_PK_EC) {
		gnutls_pk_params_release(&session->key.kshare.ecdh_params);
		gnutls_pk_params_init(&session->key.kshare.ecdh_params);

		ret = _gnutls_pk_generate_keys(group->pk, group->curve,
						&session->key.kshare.ecdh_params, 1);
		if (ret < 0)
			return gnutls_assert_val(ret);

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

		session->key.kshare.ecdh_params.algo = group->pk;
		session->key.kshare.ecdh_params.curve = group->curve;

		ret = 0;

	} else if (group->pk == GNUTLS_PK_ECDH_X25519 ||
		   group->pk == GNUTLS_PK_ECDH_X448) {
		gnutls_pk_params_release(&session->key.kshare.ecdhx_params);
		gnutls_pk_params_init(&session->key.kshare.ecdhx_params);

		ret = _gnutls_pk_generate_keys(group->pk, group->curve,
						&session->key.kshare.ecdhx_params, 1);
		if (ret < 0)
			return gnutls_assert_val(ret);

		ret =
		    _gnutls_buffer_append_data_prefix(extdata, 16,
				session->key.kshare.ecdhx_params.raw_pub.data,
				session->key.kshare.ecdhx_params.raw_pub.size);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		session->key.kshare.ecdhx_params.algo = group->pk;
		session->key.kshare.ecdhx_params.curve = group->curve;

		ret = 0;

	} else if (group->pk == GNUTLS_PK_DH) {
		/* we need to initialize the group parameters first */
		gnutls_pk_params_release(&session->key.kshare.dh_params);
		gnutls_pk_params_init(&session->key.kshare.dh_params);

		ret = _gnutls_mpi_init_scan_nz(&session->key.kshare.dh_params.params[DH_G],
			group->generator->data, group->generator->size);
		if (ret < 0)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		ret = _gnutls_mpi_init_scan_nz(&session->key.kshare.dh_params.params[DH_P],
			group->prime->data, group->prime->size);
		if (ret < 0)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		ret = _gnutls_mpi_init_scan_nz(&session->key.kshare.dh_params.params[DH_Q],
			group->q->data, group->q->size);
		if (ret < 0)
			return gnutls_assert_val(GNUTLS_E_RECEIVED_ILLEGAL_PARAMETER);

		session->key.kshare.dh_params.algo = group->pk;
		session->key.kshare.dh_params.dh_group = group->id; /* no curve in FFDH, we write the group */
		session->key.kshare.dh_params.qbits = *group->q_bits;
		session->key.kshare.dh_params.params_nr = 3;

		ret = _gnutls_pk_generate_keys(group->pk, 0, &session->key.kshare.dh_params, 1);
		if (ret < 0)
			return gnutls_assert_val(ret);

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