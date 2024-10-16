static int gf2m_Mxy(const EC_GROUP *group, const BIGNUM *x, const BIGNUM *y, BIGNUM *x1, 
	BIGNUM *z1, BIGNUM *x2, BIGNUM *z2, BN_CTX *ctx)
	{
	BIGNUM *t3, *t4, *t5;
	int ret = 0;
	
	if (BN_is_zero(z1))
		{
		BN_zero(x2);
		BN_zero(z2);
		return 1;
		}
	
	if (BN_is_zero(z2))
		{
		if (!BN_copy(x2, x)) return 0;
		if (!BN_GF2m_add(z2, x, y)) return 0;
		return 2;
		}
		
	/* Since Mxy is static we can guarantee that ctx != NULL. */
	BN_CTX_start(ctx);
	t3 = BN_CTX_get(ctx);
	t4 = BN_CTX_get(ctx);
	t5 = BN_CTX_get(ctx);
	if (t5 == NULL) goto err;

	if (!BN_one(t5)) goto err;

	if (!group->meth->field_mul(group, t3, z1, z2, ctx)) goto err;

	if (!group->meth->field_mul(group, z1, z1, x, ctx)) goto err;
	if (!BN_GF2m_add(z1, z1, x1)) goto err;
	if (!group->meth->field_mul(group, z2, z2, x, ctx)) goto err;
	if (!group->meth->field_mul(group, x1, z2, x1, ctx)) goto err;
	if (!BN_GF2m_add(z2, z2, x2)) goto err;

	if (!group->meth->field_mul(group, z2, z2, z1, ctx)) goto err;
	if (!group->meth->field_sqr(group, t4, x, ctx)) goto err;
	if (!BN_GF2m_add(t4, t4, y)) goto err;
	if (!group->meth->field_mul(group, t4, t4, t3, ctx)) goto err;
	if (!BN_GF2m_add(t4, t4, z2)) goto err;

	if (!group->meth->field_mul(group, t3, t3, x, ctx)) goto err;
	if (!group->meth->field_div(group, t3, t5, t3, ctx)) goto err;
	if (!group->meth->field_mul(group, t4, t3, t4, ctx)) goto err;
	if (!group->meth->field_mul(group, x2, x1, t3, ctx)) goto err;
	if (!BN_GF2m_add(z2, x2, x)) goto err;

	if (!group->meth->field_mul(group, z2, z2, t4, ctx)) goto err;
	if (!BN_GF2m_add(z2, z2, y)) goto err;

	ret = 2;

 err:
	BN_CTX_end(ctx);
	return ret;
	}