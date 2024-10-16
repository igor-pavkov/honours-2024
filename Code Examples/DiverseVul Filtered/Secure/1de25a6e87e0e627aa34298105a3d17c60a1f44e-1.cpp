static void inflate_stored_setup(STATE_PARAM int my_n, int my_b, int my_k)
{
	inflate_stored_n = my_n;
	inflate_stored_b = my_b;
	inflate_stored_k = my_k;
	/* initialize gunzip_window position */
	inflate_stored_w = gunzip_outbuf_count;
}