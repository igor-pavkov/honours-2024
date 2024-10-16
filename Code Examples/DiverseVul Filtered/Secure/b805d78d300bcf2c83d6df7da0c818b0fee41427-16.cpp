static int xfrm_add_pol_expire(struct sk_buff *skb, struct nlmsghdr *nlh,
		struct nlattr **attrs)
{
	struct net *net = sock_net(skb->sk);
	struct xfrm_policy *xp;
	struct xfrm_user_polexpire *up = nlmsg_data(nlh);
	struct xfrm_userpolicy_info *p = &up->pol;
	u8 type = XFRM_POLICY_TYPE_MAIN;
	int err = -ENOENT;
	struct xfrm_mark m;
	u32 mark = xfrm_mark_get(attrs, &m);
	u32 if_id = 0;

	err = copy_from_user_policy_type(&type, attrs);
	if (err)
		return err;

	err = verify_policy_dir(p->dir);
	if (err)
		return err;

	if (attrs[XFRMA_IF_ID])
		if_id = nla_get_u32(attrs[XFRMA_IF_ID]);

	if (p->index)
		xp = xfrm_policy_byid(net, mark, if_id, type, p->dir, p->index, 0, &err);
	else {
		struct nlattr *rt = attrs[XFRMA_SEC_CTX];
		struct xfrm_sec_ctx *ctx;

		err = verify_sec_ctx_len(attrs);
		if (err)
			return err;

		ctx = NULL;
		if (rt) {
			struct xfrm_user_sec_ctx *uctx = nla_data(rt);

			err = security_xfrm_policy_alloc(&ctx, uctx, GFP_KERNEL);
			if (err)
				return err;
		}
		xp = xfrm_policy_bysel_ctx(net, mark, if_id, type, p->dir,
					   &p->sel, ctx, 0, &err);
		security_xfrm_policy_free(ctx);
	}
	if (xp == NULL)
		return -ENOENT;

	if (unlikely(xp->walk.dead))
		goto out;

	err = 0;
	if (up->hard) {
		xfrm_policy_delete(xp, p->dir);
		xfrm_audit_policy_delete(xp, 1, true);
	}
	km_policy_expired(xp, p->dir, up->hard, nlh->nlmsg_pid);

out:
	xfrm_pol_put(xp);
	return err;
}