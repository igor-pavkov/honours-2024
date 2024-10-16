int compute_scalar_element (pwd_session_t *session, BN_CTX *bnctx) {
	BIGNUM *mask = NULL;
	int ret = -1;

	if (((session->private_value = BN_new()) == NULL) ||
	    ((session->my_element = EC_POINT_new(session->group)) == NULL) ||
	    ((session->my_scalar = BN_new()) == NULL) ||
	    ((mask = BN_new()) == NULL)) {
		DEBUG2("server scalar allocation failed");
		goto fail;
	}

	if (BN_rand_range(session->private_value, session->order) != 1) {
		DEBUG2("Unable to get randomness for private_value");
		goto fail;
	}
	if (BN_rand_range(mask, session->order) != 1) {
		DEBUG2("Unable to get randomness for mask");
		goto fail;
	}
	BN_add(session->my_scalar, session->private_value, mask);
	BN_mod(session->my_scalar, session->my_scalar, session->order, bnctx);

	if (!EC_POINT_mul(session->group, session->my_element, NULL, session->pwe, mask, bnctx)) {
		DEBUG2("server element allocation failed");
		goto fail;
	}

	if (!EC_POINT_invert(session->group, session->my_element, bnctx)) {
		DEBUG2("server element inversion failed");
		goto fail;
	}

	ret = 0;

fail:
	BN_clear_free(mask);

	return ret;
}