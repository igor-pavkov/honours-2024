static void alarm_handler(int sig UNUSED_PARAM)
{
	/* This is theoretically unsafe (uses stdio and malloc in signal handler) */
	if (G.die_if_timed_out)
		bb_simple_error_msg_and_die("download timed out");
}