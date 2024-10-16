static int sqfs_calc_n_blks(__le64 start, __le64 end, u64 *offset)
{
	u64 start_, table_size;

	table_size = le64_to_cpu(end) - le64_to_cpu(start);
	start_ = le64_to_cpu(start) / ctxt.cur_dev->blksz;
	*offset = le64_to_cpu(start) - (start_ * ctxt.cur_dev->blksz);

	return DIV_ROUND_UP(table_size + *offset, ctxt.cur_dev->blksz);
}