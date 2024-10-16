static int ebt_size_mwt(struct compat_ebt_entry_mwt *match32,
			unsigned int size_left, enum compat_mwt type,
			struct ebt_entries_buf_state *state, const void *base)
{
	int growth = 0;
	char *buf;

	if (size_left == 0)
		return 0;

	buf = (char *) match32;

	while (size_left >= sizeof(*match32)) {
		struct ebt_entry_match *match_kern;
		int ret;

		match_kern = (struct ebt_entry_match *) state->buf_kern_start;
		if (match_kern) {
			char *tmp;
			tmp = state->buf_kern_start + state->buf_kern_offset;
			match_kern = (struct ebt_entry_match *) tmp;
		}
		ret = ebt_buf_add(state, buf, sizeof(*match32));
		if (ret < 0)
			return ret;
		size_left -= sizeof(*match32);

		/* add padding before match->data (if any) */
		ret = ebt_buf_add_pad(state, ebt_compat_entry_padsize());
		if (ret < 0)
			return ret;

		if (match32->match_size > size_left)
			return -EINVAL;

		size_left -= match32->match_size;

		ret = compat_mtw_from_user(match32, type, state, base);
		if (ret < 0)
			return ret;

		BUG_ON(ret < match32->match_size);
		growth += ret - match32->match_size;
		growth += ebt_compat_entry_padsize();

		buf += sizeof(*match32);
		buf += match32->match_size;

		if (match_kern)
			match_kern->match_size = ret;

		WARN_ON(type == EBT_COMPAT_TARGET && size_left);
		match32 = (struct compat_ebt_entry_mwt *) buf;
	}

	return growth;
}