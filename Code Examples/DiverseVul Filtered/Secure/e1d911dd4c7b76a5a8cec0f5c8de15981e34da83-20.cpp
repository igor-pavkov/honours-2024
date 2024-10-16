static enum interesting do_match(const struct name_entry *entry,
				 struct strbuf *base, int base_offset,
				 const struct pathspec *ps,
				 int exclude)
{
	int i;
	int pathlen, baselen = base->len - base_offset;
	enum interesting never_interesting = ps->has_wildcard ?
		entry_not_interesting : all_entries_not_interesting;

	GUARD_PATHSPEC(ps,
		       PATHSPEC_FROMTOP |
		       PATHSPEC_MAXDEPTH |
		       PATHSPEC_LITERAL |
		       PATHSPEC_GLOB |
		       PATHSPEC_ICASE |
		       PATHSPEC_EXCLUDE);

	if (!ps->nr) {
		if (!ps->recursive ||
		    !(ps->magic & PATHSPEC_MAXDEPTH) ||
		    ps->max_depth == -1)
			return all_entries_interesting;
		return within_depth(base->buf + base_offset, baselen,
				    !!S_ISDIR(entry->mode),
				    ps->max_depth) ?
			entry_interesting : entry_not_interesting;
	}

	pathlen = tree_entry_len(entry);

	for (i = ps->nr - 1; i >= 0; i--) {
		const struct pathspec_item *item = ps->items+i;
		const char *match = item->match;
		const char *base_str = base->buf + base_offset;
		int matchlen = item->len, matched = 0;

		if ((!exclude &&   item->magic & PATHSPEC_EXCLUDE) ||
		    ( exclude && !(item->magic & PATHSPEC_EXCLUDE)))
			continue;

		if (baselen >= matchlen) {
			/* If it doesn't match, move along... */
			if (!match_dir_prefix(item, base_str, match, matchlen))
				goto match_wildcards;

			if (!ps->recursive ||
			    !(ps->magic & PATHSPEC_MAXDEPTH) ||
			    ps->max_depth == -1)
				return all_entries_interesting;

			return within_depth(base_str + matchlen + 1,
					    baselen - matchlen - 1,
					    !!S_ISDIR(entry->mode),
					    ps->max_depth) ?
				entry_interesting : entry_not_interesting;
		}

		/* Either there must be no base, or the base must match. */
		if (baselen == 0 || !basecmp(item, base_str, match, baselen)) {
			if (match_entry(item, entry, pathlen,
					match + baselen, matchlen - baselen,
					&never_interesting))
				return entry_interesting;

			if (item->nowildcard_len < item->len) {
				if (!git_fnmatch(item, match + baselen, entry->path,
						 item->nowildcard_len - baselen))
					return entry_interesting;

				/*
				 * Match all directories. We'll try to
				 * match files later on.
				 */
				if (ps->recursive && S_ISDIR(entry->mode))
					return entry_interesting;

				/*
				 * When matching against submodules with
				 * wildcard characters, ensure that the entry
				 * at least matches up to the first wild
				 * character.  More accurate matching can then
				 * be performed in the submodule itself.
				 */
				if (ps->recursive && S_ISGITLINK(entry->mode) &&
				    !ps_strncmp(item, match + baselen,
						entry->path,
						item->nowildcard_len - baselen))
					return entry_interesting;
			}

			continue;
		}

match_wildcards:
		if (item->nowildcard_len == item->len)
			continue;

		if (item->nowildcard_len &&
		    !match_wildcard_base(item, base_str, baselen, &matched))
			continue;

		/*
		 * Concatenate base and entry->path into one and do
		 * fnmatch() on it.
		 *
		 * While we could avoid concatenation in certain cases
		 * [1], which saves a memcpy and potentially a
		 * realloc, it turns out not worth it. Measurement on
		 * linux-2.6 does not show any clear improvements,
		 * partly because of the nowildcard_len optimization
		 * in git_fnmatch(). Avoid micro-optimizations here.
		 *
		 * [1] if match_wildcard_base() says the base
		 * directory is already matched, we only need to match
		 * the rest, which is shorter so _in theory_ faster.
		 */

		strbuf_add(base, entry->path, pathlen);

		if (!git_fnmatch(item, match, base->buf + base_offset,
				 item->nowildcard_len)) {
			strbuf_setlen(base, base_offset + baselen);
			return entry_interesting;
		}

		/*
		 * When matching against submodules with
		 * wildcard characters, ensure that the entry
		 * at least matches up to the first wild
		 * character.  More accurate matching can then
		 * be performed in the submodule itself.
		 */
		if (ps->recursive && S_ISGITLINK(entry->mode) &&
		    !ps_strncmp(item, match, base->buf + base_offset,
				item->nowildcard_len)) {
			strbuf_setlen(base, base_offset + baselen);
			return entry_interesting;
		}

		strbuf_setlen(base, base_offset + baselen);

		/*
		 * Match all directories. We'll try to match files
		 * later on.
		 * max_depth is ignored but we may consider support it
		 * in future, see
		 * https://public-inbox.org/git/7vmxo5l2g4.fsf@alter.siamese.dyndns.org/
		 */
		if (ps->recursive && S_ISDIR(entry->mode))
			return entry_interesting;
	}
	return never_interesting; /* No matches */
}