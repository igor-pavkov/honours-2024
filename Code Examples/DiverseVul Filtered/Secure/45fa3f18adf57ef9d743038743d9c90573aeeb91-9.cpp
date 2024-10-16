static void reset_beg_range_to_zero(void)
{
	bb_simple_error_msg("restart failed");
	G.beg_range = 0;
	xlseek(G.output_fd, 0, SEEK_SET);
	/* Done at the end instead: */
	/* ftruncate(G.output_fd, 0); */
}