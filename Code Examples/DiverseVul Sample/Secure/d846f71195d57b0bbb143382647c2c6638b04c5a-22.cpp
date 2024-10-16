static int compat_mtw_from_user(struct compat_ebt_entry_mwt *mwt,
				enum compat_mwt compat_mwt,
				struct ebt_entries_buf_state *state,
				const unsigned char *base)
{
	char name[EBT_FUNCTION_MAXNAMELEN];
	struct xt_match *match;
	struct xt_target *wt;
	void *dst = NULL;
	int off, pad = 0, ret = 0;
	unsigned int size_kern, entry_offset, match_size = mwt->match_size;

	strlcpy(name, mwt->u.name, sizeof(name));

	if (state->buf_kern_start)
		dst = state->buf_kern_start + state->buf_kern_offset;

	entry_offset = (unsigned char *) mwt - base;
	switch (compat_mwt) {
	case EBT_COMPAT_MATCH:
		match = try_then_request_module(xt_find_match(NFPROTO_BRIDGE,
						name, 0), "ebt_%s", name);
		if (match == NULL)
			return -ENOENT;
		if (IS_ERR(match))
			return PTR_ERR(match);

		off = ebt_compat_match_offset(match, match_size);
		if (dst) {
			if (match->compat_from_user)
				match->compat_from_user(dst, mwt->data);
			else
				memcpy(dst, mwt->data, match_size);
		}

		size_kern = match->matchsize;
		if (unlikely(size_kern == -1))
			size_kern = match_size;
		module_put(match->me);
		break;
	case EBT_COMPAT_WATCHER: /* fallthrough */
	case EBT_COMPAT_TARGET:
		wt = try_then_request_module(xt_find_target(NFPROTO_BRIDGE,
						name, 0), "ebt_%s", name);
		if (wt == NULL)
			return -ENOENT;
		if (IS_ERR(wt))
			return PTR_ERR(wt);
		off = xt_compat_target_offset(wt);

		if (dst) {
			if (wt->compat_from_user)
				wt->compat_from_user(dst, mwt->data);
			else
				memcpy(dst, mwt->data, match_size);
		}

		size_kern = wt->targetsize;
		module_put(wt->me);
		break;
	}

	if (!dst) {
		ret = xt_compat_add_offset(NFPROTO_BRIDGE, entry_offset,
					off + ebt_compat_entry_padsize());
		if (ret < 0)
			return ret;
	}

	state->buf_kern_offset += match_size + off;
	state->buf_user_offset += match_size;
	pad = XT_ALIGN(size_kern) - size_kern;

	if (pad > 0 && dst) {
		BUG_ON(state->buf_kern_len <= pad);
		BUG_ON(state->buf_kern_offset - (match_size + off) + size_kern > state->buf_kern_len - pad);
		memset(dst + size_kern, 0, pad);
	}
	return off + match_size;
}