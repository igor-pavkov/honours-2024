static void cancel_activity(void)
{
	do_floppy = NULL;
	cancel_delayed_work(&fd_timer);
	cancel_work_sync(&floppy_work);
}