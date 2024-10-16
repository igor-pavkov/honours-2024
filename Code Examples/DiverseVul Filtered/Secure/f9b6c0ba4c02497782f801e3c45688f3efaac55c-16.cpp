int ec_GF2m_simple_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *scalar,
	size_t num, const EC_POINT *points[], const BIGNUM *scalars[], BN_CTX *ctx)
	{
	BN_CTX *new_ctx = NULL;
	int ret = 0;
	size_t i;
	EC_POINT *p=NULL;
	EC_POINT *acc = NULL;

	if (ctx == NULL)
		{
		ctx = new_ctx = BN_CTX_new();
		if (ctx == NULL)
			return 0;
		}

	/* This implementation is more efficient than the wNAF implementation for 2
	 * or fewer points.  Use the ec_wNAF_mul implementation for 3 or more points,
	 * or if we can perform a fast multiplication based on precomputation.
	 */
	if ((scalar && (num > 1)) || (num > 2) || (num == 0 && EC_GROUP_have_precompute_mult(group)))
		{
		ret = ec_wNAF_mul(group, r, scalar, num, points, scalars, ctx);
		goto err;
		}

	if ((p = EC_POINT_new(group)) == NULL) goto err;
	if ((acc = EC_POINT_new(group)) == NULL) goto err;

	if (!EC_POINT_set_to_infinity(group, acc)) goto err;

	if (scalar)
		{
		if (!ec_GF2m_montgomery_point_multiply(group, p, scalar, group->generator, ctx)) goto err;
		if (BN_is_negative(scalar))
			if (!group->meth->invert(group, p, ctx)) goto err;
		if (!group->meth->add(group, acc, acc, p, ctx)) goto err;
		}

	for (i = 0; i < num; i++)
		{
		if (!ec_GF2m_montgomery_point_multiply(group, p, scalars[i], points[i], ctx)) goto err;
		if (BN_is_negative(scalars[i]))
			if (!group->meth->invert(group, p, ctx)) goto err;
		if (!group->meth->add(group, acc, acc, p, ctx)) goto err;
		}

	if (!EC_POINT_copy(r, acc)) goto err;

	ret = 1;

  err:
	if (p) EC_POINT_free(p);
	if (acc) EC_POINT_free(acc);
	if (new_ctx != NULL)
		BN_CTX_free(new_ctx);
	return ret;
	}