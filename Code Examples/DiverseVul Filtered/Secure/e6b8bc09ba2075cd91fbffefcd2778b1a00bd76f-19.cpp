static int dx_make_map (struct ext4_dir_entry_2 *de, int size,
			struct dx_hash_info *hinfo, struct dx_map_entry *map_tail)
{
	int count = 0;
	char *base = (char *) de;
	struct dx_hash_info h = *hinfo;

	while ((char *) de < base + size)
	{
		if (de->name_len && de->inode) {
			ext4fs_dirhash(de->name, de->name_len, &h);
			map_tail--;
			map_tail->hash = h.hash;
			map_tail->offs = (u16) ((char *) de - base);
			map_tail->size = le16_to_cpu(de->rec_len);
			count++;
			cond_resched();
		}
		/* XXX: do we need to check rec_len == 0 case? -Chris */
		de = ext4_next_entry(de);
	}
	return count;
}