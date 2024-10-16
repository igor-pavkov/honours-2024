static int getkeycode(unsigned int scancode)
{
	struct getset_keycode_data d = {
		.ke	= {
			.flags		= 0,
			.len		= sizeof(scancode),
			.keycode	= 0,
		},
		.error	= -ENODEV,
	};

	memcpy(d.ke.scancode, &scancode, sizeof(scancode));

	input_handler_for_each_handle(&kbd_handler, &d, getkeycode_helper);

	return d.error ?: d.ke.keycode;
}