static void comedi_cleanup_legacy_minors(void)
{
	unsigned i;

	for (i = 0; i < comedi_num_legacy_minors; i++)
		comedi_free_board_minor(i);
}