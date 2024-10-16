static inline unsigned char echo_buf(struct n_tty_data *ldata, size_t i)
{
	return ldata->echo_buf[i & (N_TTY_BUF_SIZE - 1)];
}