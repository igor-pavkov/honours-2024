int hidp_add_connection(struct hidp_connadd_req *req, struct socket *ctrl_sock, struct socket *intr_sock)
{
	struct hidp_session *session, *s;
	int vendor, product;
	int err;

	BT_DBG("");

	if (bacmp(&bt_sk(ctrl_sock->sk)->src, &bt_sk(intr_sock->sk)->src) ||
			bacmp(&bt_sk(ctrl_sock->sk)->dst, &bt_sk(intr_sock->sk)->dst))
		return -ENOTUNIQ;

	BT_DBG("rd_data %p rd_size %d", req->rd_data, req->rd_size);

	down_write(&hidp_session_sem);

	s = __hidp_get_session(&bt_sk(ctrl_sock->sk)->dst);
	if (s && s->state == BT_CONNECTED) {
		up_write(&hidp_session_sem);
		return -EEXIST;
	}

	session = kzalloc(sizeof(struct hidp_session), GFP_KERNEL);
	if (!session) {
		up_write(&hidp_session_sem);
		return -ENOMEM;
	}

	bacpy(&session->bdaddr, &bt_sk(ctrl_sock->sk)->dst);

	session->ctrl_mtu = min_t(uint, l2cap_pi(ctrl_sock->sk)->chan->omtu,
					l2cap_pi(ctrl_sock->sk)->chan->imtu);
	session->intr_mtu = min_t(uint, l2cap_pi(intr_sock->sk)->chan->omtu,
					l2cap_pi(intr_sock->sk)->chan->imtu);

	BT_DBG("ctrl mtu %d intr mtu %d", session->ctrl_mtu, session->intr_mtu);

	session->ctrl_sock = ctrl_sock;
	session->intr_sock = intr_sock;
	session->state     = BT_CONNECTED;

	session->conn = hidp_get_connection(session);
	if (!session->conn) {
		err = -ENOTCONN;
		goto failed;
	}

	setup_timer(&session->timer, hidp_idle_timeout, (unsigned long)session);

	skb_queue_head_init(&session->ctrl_transmit);
	skb_queue_head_init(&session->intr_transmit);

	mutex_init(&session->report_mutex);
	init_waitqueue_head(&session->report_queue);
	init_waitqueue_head(&session->startup_queue);
	session->waiting_for_startup = 1;
	session->flags   = req->flags & (1 << HIDP_BLUETOOTH_VENDOR_ID);
	session->idle_to = req->idle_to;

	__hidp_link_session(session);

	if (req->rd_size > 0) {
		err = hidp_setup_hid(session, req);
		if (err && err != -ENODEV)
			goto purge;
	}

	if (!session->hid) {
		err = hidp_setup_input(session, req);
		if (err < 0)
			goto purge;
	}

	hidp_set_timer(session);

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

	session->task = kthread_run(hidp_session, session, "khidpd_%04x%04x",
							vendor, product);
	if (IS_ERR(session->task)) {
		err = PTR_ERR(session->task);
		goto unlink;
	}

	while (session->waiting_for_startup) {
		wait_event_interruptible(session->startup_queue,
			!session->waiting_for_startup);
	}

	if (session->hid)
		err = hid_add_device(session->hid);
	else
		err = input_register_device(session->input);

	if (err < 0) {
		atomic_inc(&session->terminate);
		wake_up_process(session->task);
		up_write(&hidp_session_sem);
		return err;
	}

	if (session->input) {
		hidp_send_ctrl_message(session,
			HIDP_TRANS_SET_PROTOCOL | HIDP_PROTO_BOOT, NULL, 0);
		session->flags |= (1 << HIDP_BOOT_PROTOCOL_MODE);

		session->leds = 0xff;
		hidp_input_event(session->input, EV_LED, 0, 0);
	}

	up_write(&hidp_session_sem);
	return 0;

unlink:
	hidp_del_timer(session);

	if (session->input) {
		input_unregister_device(session->input);
		session->input = NULL;
	}

	if (session->hid) {
		hid_destroy_device(session->hid);
		session->hid = NULL;
	}

	kfree(session->rd_data);
	session->rd_data = NULL;

purge:
	__hidp_unlink_session(session);

	skb_queue_purge(&session->ctrl_transmit);
	skb_queue_purge(&session->intr_transmit);

failed:
	up_write(&hidp_session_sem);

	kfree(session);
	return err;
}