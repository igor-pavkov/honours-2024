static int gf2m_Mdouble(const EC_GROUP *group, BIGNUM *x, BIGNUM *z, BN_CTX *ctx)
	{
	BIGNUM *t1;
	int ret = 0;
	
	/* Since Mdouble is static we can guarantee that ctx != NULL. */
	BN_CTX_start(ctx);
	t1 = BN_CTX_get(ctx);
	if (t1 == NULL) goto err;

	if (!group->meth->field_sqr(group, x, x, ctx)) goto err;
	if (!group->meth->field_sqr(group, t1, z, ctx)) goto err;
	if (!group->meth->field_mul(group, z, x, t1, ctx)) goto err;
	if (!group->meth->field_sqr(group, x, x, ctx)) goto err;
	if (!group->meth->field_sqr(group, t1, t1, ctx)) goto err;
	if (!group->meth->field_mul(group, t1, &group->b, t1, ctx)) goto err;
	if (!BN_GF2m_add(x, x, t1)) goto err;

	ret = 1;

 err:
	BN_CTX_end(ctx);
	return ret;
	}