static inline void add_echo_byte(unsigned char c, struct n_tty_data *ldata)
{
	*echo_buf_addr(ldata, ldata->echo_head++) = c;
}