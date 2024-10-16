static int hidp_setup_hid(struct hidp_session *session,
				struct hidp_connadd_req *req)
{
	struct hid_device *hid;
	int err;

	session->rd_data = memdup_user(req->rd_data, req->rd_size);
	if (IS_ERR(session->rd_data))
		return PTR_ERR(session->rd_data);

	session->rd_size = req->rd_size;

	hid = hid_allocate_device();
	if (IS_ERR(hid)) {
		err = PTR_ERR(hid);
		goto fault;
	}

	session->hid = hid;

	hid->driver_data = session;

	hid->bus     = BUS_BLUETOOTH;
	hid->vendor  = req->vendor;
	hid->product = req->product;
	hid->version = req->version;
	hid->country = req->country;

	strncpy(hid->name, req->name, sizeof(req->name) - 1);

	snprintf(hid->phys, sizeof(hid->phys), "%pMR",
		 &l2cap_pi(session->ctrl_sock->sk)->chan->src);

	/* NOTE: Some device modules depend on the dst address being stored in
	 * uniq. Please be aware of this before making changes to this behavior.
	 */
	snprintf(hid->uniq, sizeof(hid->uniq), "%pMR",
		 &l2cap_pi(session->ctrl_sock->sk)->chan->dst);

	hid->dev.parent = &session->conn->hcon->dev;
	hid->ll_driver = &hidp_hid_driver;

	/* True if device is blacklisted in drivers/hid/hid-core.c */
	if (hid_ignore(hid)) {
		hid_destroy_device(session->hid);
		session->hid = NULL;
		return -ENODEV;
	}

	return 0;

fault:
	kfree(session->rd_data);
	session->rd_data = NULL;

	return err;
}