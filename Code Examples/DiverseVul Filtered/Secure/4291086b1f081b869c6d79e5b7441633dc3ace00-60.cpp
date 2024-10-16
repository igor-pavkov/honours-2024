static inline unsigned char read_buf(struct n_tty_data *ldata, size_t i)
{
	return ldata->read_buf[i & (N_TTY_BUF_SIZE - 1)];
}