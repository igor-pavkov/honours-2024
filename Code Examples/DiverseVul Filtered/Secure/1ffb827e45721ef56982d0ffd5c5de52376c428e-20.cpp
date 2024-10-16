int _gnutls_dh_set_secret_bits(gnutls_session_t session, unsigned bits)
{
	switch (gnutls_auth_get_type(session)) {
	case GNUTLS_CRD_ANON:
		{
			anon_auth_info_t info;
			info = _gnutls_get_auth_info(session, GNUTLS_CRD_ANON);
			if (info == NULL)
				return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
			info->dh.secret_bits = bits;
			break;
		}
	case GNUTLS_CRD_PSK:
		{
			psk_auth_info_t info;
			info = _gnutls_get_auth_info(session, GNUTLS_CRD_PSK);
			if (info == NULL)
				return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
			info->dh.secret_bits = bits;
			break;
		}
	case GNUTLS_CRD_CERTIFICATE:
		{
			cert_auth_info_t info;

			info = _gnutls_get_auth_info(session, GNUTLS_CRD_CERTIFICATE);
			if (info == NULL)
				return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

			info->dh.secret_bits = bits;
			break;
	default:
			return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
		}
	}

	return 0;
}