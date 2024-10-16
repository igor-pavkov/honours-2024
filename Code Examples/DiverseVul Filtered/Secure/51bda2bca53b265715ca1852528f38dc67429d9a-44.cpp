static int hidp_session_dev_add(struct hidp_session *session)
{
	int ret;

	/* Both HID and input systems drop a ref-count when unregistering the
	 * device but they don't take a ref-count when registering them. Work
	 * around this by explicitly taking a refcount during registration
	 * which is dropped automatically by unregistering the devices. */

	if (session->hid) {
		ret = hid_add_device(session->hid);
		if (ret)
			return ret;
		get_device(&session->hid->dev);
	} else if (session->input) {
		ret = input_register_device(session->input);
		if (ret)
			return ret;
		input_get_device(session->input);
	}

	return 0;
}