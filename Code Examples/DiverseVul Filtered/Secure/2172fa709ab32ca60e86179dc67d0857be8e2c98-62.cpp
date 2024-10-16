int security_netlbl_secattr_to_sid(struct netlbl_lsm_secattr *secattr,
				   u32 *sid)
{
	int rc;
	struct context *ctx;
	struct context ctx_new;

	if (!ss_initialized) {
		*sid = SECSID_NULL;
		return 0;
	}

	read_lock(&policy_rwlock);

	if (secattr->flags & NETLBL_SECATTR_CACHE)
		*sid = *(u32 *)secattr->cache->data;
	else if (secattr->flags & NETLBL_SECATTR_SECID)
		*sid = secattr->attr.secid;
	else if (secattr->flags & NETLBL_SECATTR_MLS_LVL) {
		rc = -EIDRM;
		ctx = sidtab_search(&sidtab, SECINITSID_NETMSG);
		if (ctx == NULL)
			goto out;

		context_init(&ctx_new);
		ctx_new.user = ctx->user;
		ctx_new.role = ctx->role;
		ctx_new.type = ctx->type;
		mls_import_netlbl_lvl(&ctx_new, secattr);
		if (secattr->flags & NETLBL_SECATTR_MLS_CAT) {
			rc = ebitmap_netlbl_import(&ctx_new.range.level[0].cat,
						   secattr->attr.mls.cat);
			if (rc)
				goto out;
			memcpy(&ctx_new.range.level[1].cat,
			       &ctx_new.range.level[0].cat,
			       sizeof(ctx_new.range.level[0].cat));
		}
		rc = -EIDRM;
		if (!mls_context_isvalid(&policydb, &ctx_new))
			goto out_free;

		rc = sidtab_context_to_sid(&sidtab, &ctx_new, sid);
		if (rc)
			goto out_free;

		security_netlbl_cache_add(secattr, *sid);

		ebitmap_destroy(&ctx_new.range.level[0].cat);
	} else
		*sid = SECSID_NULL;

	read_unlock(&policy_rwlock);
	return 0;
out_free:
	ebitmap_destroy(&ctx_new.range.level[0].cat);
out:
	read_unlock(&policy_rwlock);
	return rc;
}