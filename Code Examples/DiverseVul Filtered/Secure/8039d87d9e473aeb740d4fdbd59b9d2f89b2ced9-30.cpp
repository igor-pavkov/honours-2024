static int should_defrag_range(struct inode *inode, u64 start, u32 thresh,
			       u64 *last_len, u64 *skip, u64 *defrag_end,
			       int compress)
{
	struct extent_map *em;
	int ret = 1;
	bool next_mergeable = true;
	bool prev_mergeable = true;

	/*
	 * make sure that once we start defragging an extent, we keep on
	 * defragging it
	 */
	if (start < *defrag_end)
		return 1;

	*skip = 0;

	em = defrag_lookup_extent(inode, start);
	if (!em)
		return 0;

	/* this will cover holes, and inline extents */
	if (em->block_start >= EXTENT_MAP_LAST_BYTE) {
		ret = 0;
		goto out;
	}

	if (!*defrag_end)
		prev_mergeable = false;

	next_mergeable = defrag_check_next_extent(inode, em);
	/*
	 * we hit a real extent, if it is big or the next extent is not a
	 * real extent, don't bother defragging it
	 */
	if (!compress && (*last_len == 0 || *last_len >= thresh) &&
	    (em->len >= thresh || (!next_mergeable && !prev_mergeable)))
		ret = 0;
out:
	/*
	 * last_len ends up being a counter of how many bytes we've defragged.
	 * every time we choose not to defrag an extent, we reset *last_len
	 * so that the next tiny extent will force a defrag.
	 *
	 * The end result of this is that tiny extents before a single big
	 * extent will force at least part of that big extent to be defragged.
	 */
	if (ret) {
		*defrag_end = extent_map_end(em);
	} else {
		*last_len = 0;
		*skip = extent_map_end(em);
		*defrag_end = 0;
	}

	free_extent_map(em);
	return ret;
}