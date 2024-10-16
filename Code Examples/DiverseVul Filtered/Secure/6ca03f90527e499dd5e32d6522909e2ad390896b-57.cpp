static int setkeycode(unsigned int scancode, unsigned int keycode)
{
	struct getset_keycode_data d = {
		.ke	= {
			.flags		= 0,
			.len		= sizeof(scancode),
			.keycode	= keycode,
		},
		.error	= -ENODEV,
	};

	memcpy(d.ke.scancode, &scancode, sizeof(scancode));

	input_handler_for_each_handle(&kbd_handler, &d, setkeycode_helper);

	return d.error;
}