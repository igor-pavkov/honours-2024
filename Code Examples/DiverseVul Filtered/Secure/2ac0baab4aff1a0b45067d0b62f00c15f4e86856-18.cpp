static int sqfs_count_metablks(void *table, u32 offset, int table_size)
{
	int count = 0, cur_size = 0, ret;
	u32 data_size;
	bool comp;

	do {
		ret = sqfs_read_metablock(table, offset + cur_size, &comp,
					  &data_size);
		if (ret)
			return -EINVAL;
		cur_size += data_size + SQFS_HEADER_SIZE;
		count++;
	} while (cur_size < table_size);

	return count;
}