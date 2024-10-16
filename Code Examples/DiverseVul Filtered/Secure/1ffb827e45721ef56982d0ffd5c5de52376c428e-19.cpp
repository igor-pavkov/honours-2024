int _gnutls_dh_set_peer_public(gnutls_session_t session, bigint_t public)
{
	dh_info_st *dh;
	int ret;

	switch (gnutls_auth_get_type(session)) {
	case GNUTLS_CRD_ANON:
		{
			anon_auth_info_t info;
			info = _gnutls_get_auth_info(session, GNUTLS_CRD_ANON);
			if (info == NULL)
				return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

			dh = &info->dh;
			break;
		}
	case GNUTLS_CRD_PSK:
		{
			psk_auth_info_t info;
			info = _gnutls_get_auth_info(session, GNUTLS_CRD_PSK);
			if (info == NULL)
				return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

			dh = &info->dh;
			break;
		}
	case GNUTLS_CRD_CERTIFICATE:
		{
			cert_auth_info_t info;

			info = _gnutls_get_auth_info(session, GNUTLS_CRD_CERTIFICATE);
			if (info == NULL)
				return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

			dh = &info->dh;
			break;
		}
	default:
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
	}

	if (dh->public_key.data)
		_gnutls_free_datum(&dh->public_key);

	ret = _gnutls_mpi_dprint_lz(public, &dh->public_key);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	return 0;
}