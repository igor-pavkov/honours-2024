static int gf2m_Madd(const EC_GROUP *group, const BIGNUM *x, BIGNUM *x1, BIGNUM *z1, 
	const BIGNUM *x2, const BIGNUM *z2, BN_CTX *ctx)
	{
	BIGNUM *t1, *t2;
	int ret = 0;
	
	/* Since Madd is static we can guarantee that ctx != NULL. */
	BN_CTX_start(ctx);
	t1 = BN_CTX_get(ctx);
	t2 = BN_CTX_get(ctx);
	if (t2 == NULL) goto err;

	if (!BN_copy(t1, x)) goto err;
	if (!group->meth->field_mul(group, x1, x1, z2, ctx)) goto err;
	if (!group->meth->field_mul(group, z1, z1, x2, ctx)) goto err;
	if (!group->meth->field_mul(group, t2, x1, z1, ctx)) goto err;
	if (!BN_GF2m_add(z1, z1, x1)) goto err;
	if (!group->meth->field_sqr(group, z1, z1, ctx)) goto err;
	if (!group->meth->field_mul(group, x1, z1, t1, ctx)) goto err;
	if (!BN_GF2m_add(x1, x1, t2)) goto err;

	ret = 1;

 err:
	BN_CTX_end(ctx);
	return ret;
	}