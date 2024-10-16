static int hidp_session_start_sync(struct hidp_session *session)
{
	unsigned int vendor, product;

	if (session->hid) {
		vendor  = session->hid->vendor;
		product = session->hid->product;
	} else if (session->input) {
		vendor  = session->input->id.vendor;
		product = session->input->id.product;
	} else {
		vendor = 0x0000;
		product = 0x0000;
	}

	session->task = kthread_run(hidp_session_thread, session,
				    "khidpd_%04x%04x", vendor, product);
	if (IS_ERR(session->task))
		return PTR_ERR(session->task);

	while (atomic_read(&session->state) <= HIDP_SESSION_IDLING)
		wait_event(session->state_queue,
			   atomic_read(&session->state) > HIDP_SESSION_IDLING);

	return 0;
}