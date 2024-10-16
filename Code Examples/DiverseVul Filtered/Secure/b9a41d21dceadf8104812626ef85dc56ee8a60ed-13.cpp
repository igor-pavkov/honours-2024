static void do_deferred_remove(struct work_struct *w)
{
	dm_deferred_remove();
}