static int hidp_session_probe(struct l2cap_conn *conn,
			      struct l2cap_user *user)
{
	struct hidp_session *session = container_of(user,
						    struct hidp_session,
						    user);
	struct hidp_session *s;
	int ret;

	down_write(&hidp_session_sem);

	/* check that no other session for this device exists */
	s = __hidp_session_find(&session->bdaddr);
	if (s) {
		ret = -EEXIST;
		goto out_unlock;
	}

	if (session->input) {
		ret = hidp_session_dev_add(session);
		if (ret)
			goto out_unlock;
	}

	ret = hidp_session_start_sync(session);
	if (ret)
		goto out_del;

	/* HID device registration is async to allow I/O during probe */
	if (session->input)
		atomic_inc(&session->state);
	else
		schedule_work(&session->dev_init);

	hidp_session_get(session);
	list_add(&session->list, &hidp_session_list);
	ret = 0;
	goto out_unlock;

out_del:
	if (session->input)
		hidp_session_dev_del(session);
out_unlock:
	up_write(&hidp_session_sem);
	return ret;
}