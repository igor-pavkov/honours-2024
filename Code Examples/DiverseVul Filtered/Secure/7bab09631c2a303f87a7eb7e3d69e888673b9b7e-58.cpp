static struct dst_entry *xfrm_bundle_create(struct xfrm_policy *policy,
					    struct xfrm_state **xfrm, int nx,
					    const struct flowi *fl,
					    struct dst_entry *dst)
{
	struct net *net = xp_net(policy);
	unsigned long now = jiffies;
	struct net_device *dev;
	struct xfrm_mode *inner_mode;
	struct dst_entry *dst_prev = NULL;
	struct dst_entry *dst0 = NULL;
	int i = 0;
	int err;
	int header_len = 0;
	int nfheader_len = 0;
	int trailer_len = 0;
	int tos;
	int family = policy->selector.family;
	xfrm_address_t saddr, daddr;

	xfrm_flowi_addr_get(fl, &saddr, &daddr, family);

	tos = xfrm_get_tos(fl, family);

	dst_hold(dst);

	for (; i < nx; i++) {
		struct xfrm_dst *xdst = xfrm_alloc_dst(net, family);
		struct dst_entry *dst1 = &xdst->u.dst;

		err = PTR_ERR(xdst);
		if (IS_ERR(xdst)) {
			dst_release(dst);
			goto put_states;
		}

		if (xfrm[i]->sel.family == AF_UNSPEC) {
			inner_mode = xfrm_ip2inner_mode(xfrm[i],
							xfrm_af2proto(family));
			if (!inner_mode) {
				err = -EAFNOSUPPORT;
				dst_release(dst);
				goto put_states;
			}
		} else
			inner_mode = xfrm[i]->inner_mode;

		if (!dst_prev)
			dst0 = dst1;
		else
			/* Ref count is taken during xfrm_alloc_dst()
			 * No need to do dst_clone() on dst1
			 */
			dst_prev->child = dst1;

		xdst->route = dst;
		dst_copy_metrics(dst1, dst);

		if (xfrm[i]->props.mode != XFRM_MODE_TRANSPORT) {
			family = xfrm[i]->props.family;
			dst = xfrm_dst_lookup(xfrm[i], tos, fl->flowi_oif,
					      &saddr, &daddr, family);
			err = PTR_ERR(dst);
			if (IS_ERR(dst))
				goto put_states;
		} else
			dst_hold(dst);

		dst1->xfrm = xfrm[i];
		xdst->xfrm_genid = xfrm[i]->genid;

		dst1->obsolete = DST_OBSOLETE_FORCE_CHK;
		dst1->flags |= DST_HOST;
		dst1->lastuse = now;

		dst1->input = dst_discard;
		dst1->output = inner_mode->afinfo->output;

		dst1->next = dst_prev;
		dst_prev = dst1;

		header_len += xfrm[i]->props.header_len;
		if (xfrm[i]->type->flags & XFRM_TYPE_NON_FRAGMENT)
			nfheader_len += xfrm[i]->props.header_len;
		trailer_len += xfrm[i]->props.trailer_len;
	}

	dst_prev->child = dst;
	dst0->path = dst;

	err = -ENODEV;
	dev = dst->dev;
	if (!dev)
		goto free_dst;

	xfrm_init_path((struct xfrm_dst *)dst0, dst, nfheader_len);
	xfrm_init_pmtu(dst_prev);

	for (dst_prev = dst0; dst_prev != dst; dst_prev = dst_prev->child) {
		struct xfrm_dst *xdst = (struct xfrm_dst *)dst_prev;

		err = xfrm_fill_dst(xdst, dev, fl);
		if (err)
			goto free_dst;

		dst_prev->header_len = header_len;
		dst_prev->trailer_len = trailer_len;
		header_len -= xdst->u.dst.xfrm->props.header_len;
		trailer_len -= xdst->u.dst.xfrm->props.trailer_len;
	}

out:
	return dst0;

put_states:
	for (; i < nx; i++)
		xfrm_state_put(xfrm[i]);
free_dst:
	if (dst0)
		dst_release_immediate(dst0);
	dst0 = ERR_PTR(err);
	goto out;
}