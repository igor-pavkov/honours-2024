static int sync_page_killable(void *word)
{
	sync_page(word);
	return fatal_signal_pending(current) ? -EINTR : 0;
}