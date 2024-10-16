gnutls_mac_algorithm_t gnutls_mac_get(gnutls_session_t session)
{
	record_parameters_st *record_params;
	int ret;

	ret =
	    _gnutls_epoch_get(session, EPOCH_READ_CURRENT, &record_params);
	if (ret < 0)
		return gnutls_assert_val(GNUTLS_MAC_NULL);

	return record_params->mac->id;
}