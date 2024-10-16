int compute_keys (pwd_session_t *session, uint8_t *peer_confirm, uint8_t *msk, uint8_t *emsk)
{
	HMAC_CTX *ctx = NULL;
	uint8_t mk[SHA256_DIGEST_LENGTH], *cruft = NULL;
	uint8_t session_id[SHA256_DIGEST_LENGTH + 1];
	uint8_t msk_emsk[128];		/* 64 each */
	int offset, ret = -1;

	ctx = HMAC_CTX_new();
	if (ctx == NULL) {
		DEBUG2("pwd: unable to allocate HMAC context!");
		goto finish;
	}

	if ((cruft = talloc_array(session, uint8_t, BN_num_bytes(session->prime))) == NULL) {
		DEBUG2("pwd: unable to allocate space to compute keys");
		goto finish;
	}

	/*
	 * first compute the session-id = TypeCode | H(ciphersuite | scal_p |
	 *	scal_s)
	 */
	session_id[0] = PW_EAP_PWD;
	H_Init(ctx);
	H_Update(ctx, (uint8_t *)&session->ciphersuite, sizeof(session->ciphersuite));
	offset = BN_num_bytes(session->order) - BN_num_bytes(session->peer_scalar);
	memset(cruft, 0, BN_num_bytes(session->prime));
	BN_bn2bin(session->peer_scalar, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->order));
	offset = BN_num_bytes(session->order) - BN_num_bytes(session->my_scalar);
	memset(cruft, 0, BN_num_bytes(session->prime));
	BN_bn2bin(session->my_scalar, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->order));
	H_Final(ctx, (uint8_t *)&session_id[1]);

	/* then compute MK = H(k | commit-peer | commit-server) */
	H_Init(ctx);

	memset(cruft, 0, BN_num_bytes(session->prime));
	offset = BN_num_bytes(session->prime) - BN_num_bytes(session->k);
	BN_bn2bin(session->k, cruft + offset);
	H_Update(ctx, cruft, BN_num_bytes(session->prime));

	H_Update(ctx, peer_confirm, SHA256_DIGEST_LENGTH);

	H_Update(ctx, session->my_confirm, SHA256_DIGEST_LENGTH);

	H_Final(ctx, mk);

	/* stretch the mk with the session-id to get MSK | EMSK */
	if (eap_pwd_kdf(mk, SHA256_DIGEST_LENGTH, (char const *)session_id,
		        SHA256_DIGEST_LENGTH + 1, msk_emsk,
			/* it's bits, ((64 + 64) * 8) */
			1024) != 0) {
		DEBUG("key derivation function failed");
		goto finish;
	}

	memcpy(msk, msk_emsk, 64);
	memcpy(emsk, msk_emsk + 64, 64);

	ret = 0;
finish:
	talloc_free(cruft);
	HMAC_CTX_free(ctx);
	return ret;
}