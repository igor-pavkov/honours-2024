static void show_floppy(int fdc)
{
	int i;

	pr_info("\n");
	pr_info("floppy driver state\n");
	pr_info("-------------------\n");
	pr_info("now=%lu last interrupt=%lu diff=%lu last called handler=%ps\n",
		jiffies, interruptjiffies, jiffies - interruptjiffies,
		lasthandler);

	pr_info("timeout_message=%s\n", timeout_message);
	pr_info("last output bytes:\n");
	for (i = 0; i < OLOGSIZE; i++)
		pr_info("%2x %2x %lu\n",
			output_log[(i + output_log_pos) % OLOGSIZE].data,
			output_log[(i + output_log_pos) % OLOGSIZE].status,
			output_log[(i + output_log_pos) % OLOGSIZE].jiffies);
	pr_info("last result at %lu\n", resultjiffies);
	pr_info("last redo_fd_request at %lu\n", lastredo);
	print_hex_dump(KERN_INFO, "", DUMP_PREFIX_NONE, 16, 1,
		       reply_buffer, resultsize, true);

	pr_info("status=%x\n", fdc_inb(fdc, FD_STATUS));
	pr_info("fdc_busy=%lu\n", fdc_busy);
	if (do_floppy)
		pr_info("do_floppy=%ps\n", do_floppy);
	if (work_pending(&floppy_work))
		pr_info("floppy_work.func=%ps\n", floppy_work.func);
	if (delayed_work_pending(&fd_timer))
		pr_info("delayed work.function=%p expires=%ld\n",
		       fd_timer.work.func,
		       fd_timer.timer.expires - jiffies);
	if (delayed_work_pending(&fd_timeout))
		pr_info("timer_function=%p expires=%ld\n",
		       fd_timeout.work.func,
		       fd_timeout.timer.expires - jiffies);

	pr_info("cont=%p\n", cont);
	pr_info("current_req=%p\n", current_req);
	pr_info("command_status=%d\n", command_status);
	pr_info("\n");
}