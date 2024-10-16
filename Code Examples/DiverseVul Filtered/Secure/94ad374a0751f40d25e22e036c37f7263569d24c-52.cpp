static void shrink_readahead_size_eio(struct file *filp,
					struct file_ra_state *ra)
{
	if (!ra->ra_pages)
		return;

	ra->ra_pages /= 4;
}