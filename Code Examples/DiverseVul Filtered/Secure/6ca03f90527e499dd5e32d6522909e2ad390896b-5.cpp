int kbd_rate(struct kbd_repeat *rpt)
{
	struct kbd_repeat data[2] = { *rpt };

	input_handler_for_each_handle(&kbd_handler, data, kbd_rate_helper);
	*rpt = data[1];	/* Copy currently used settings */

	return 0;
}