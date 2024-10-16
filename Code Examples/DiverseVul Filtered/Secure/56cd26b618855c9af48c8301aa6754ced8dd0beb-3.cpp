static void serial_ir_timeout(struct timer_list *unused)
{
	struct ir_raw_event ev = {
		.timeout = true,
		.duration = serial_ir.rcdev->timeout
	};
	ir_raw_event_store_with_filter(serial_ir.rcdev, &ev);
	ir_raw_event_handle(serial_ir.rcdev);
}