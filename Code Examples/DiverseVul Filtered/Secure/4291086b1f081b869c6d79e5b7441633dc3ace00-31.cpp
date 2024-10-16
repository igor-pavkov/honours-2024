static inline size_t read_cnt(struct n_tty_data *ldata)
{
	return ldata->read_head - ldata->read_tail;
}