static void inflate_codes_setup(STATE_PARAM unsigned my_bl, unsigned my_bd)
{
	bl = my_bl;
	bd = my_bd;
	/* make local copies of globals */
	bb = gunzip_bb;			/* initialize bit buffer */
	k = gunzip_bk;
	w = gunzip_outbuf_count;	/* initialize gunzip_window position */
	/* inflate the coded data */
	ml = mask_bits[bl];		/* precompute masks for speed */
	md = mask_bits[bd];
}