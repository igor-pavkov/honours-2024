static int sqfs_read_entry(struct squashfs_directory_entry **dest, void *src)
{
	struct squashfs_directory_entry *tmp;
	u16 sz;

	tmp = src;
	sz = get_unaligned_le16(src + sizeof(*tmp) - sizeof(u16));
	/*
	 * 'src' points to the begin of a directory entry, and 'sz' gets its
	 * 'name_size' member's value. name_size is actually the string
	 * length - 1, so adding 2 compensates this difference and adds space
	 * for the trailling null byte.
	 */
	*dest = malloc(sizeof(*tmp) + sz + 2);
	if (!*dest)
		return -ENOMEM;

	memcpy(*dest, src, sizeof(*tmp) + sz + 1);
	(*dest)->name[sz + 1] = '\0';

	return 0;
}