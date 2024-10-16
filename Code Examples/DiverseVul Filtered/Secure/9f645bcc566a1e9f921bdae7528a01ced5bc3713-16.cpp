static int uvesafb_exec(struct uvesafb_ktask *task)
{
	static int seq;
	struct cn_msg *m;
	int err;
	int len = sizeof(task->t) + task->t.buf_len;

	/*
	 * Check whether the message isn't longer than the maximum
	 * allowed by connector.
	 */
	if (sizeof(*m) + len > CONNECTOR_MAX_MSG_SIZE) {
		pr_warn("message too long (%d), can't execute task\n",
			(int)(sizeof(*m) + len));
		return -E2BIG;
	}

	m = kzalloc(sizeof(*m) + len, GFP_KERNEL);
	if (!m)
		return -ENOMEM;

	init_completion(task->done);

	memcpy(&m->id, &uvesafb_cn_id, sizeof(m->id));
	m->seq = seq;
	m->len = len;
	m->ack = prandom_u32();

	/* uvesafb_task structure */
	memcpy(m + 1, &task->t, sizeof(task->t));

	/* Buffer */
	memcpy((u8 *)(m + 1) + sizeof(task->t), task->buf, task->t.buf_len);

	/*
	 * Save the message ack number so that we can find the kernel
	 * part of this task when a reply is received from userspace.
	 */
	task->ack = m->ack;

	mutex_lock(&uvfb_lock);

	/* If all slots are taken -- bail out. */
	if (uvfb_tasks[seq]) {
		mutex_unlock(&uvfb_lock);
		err = -EBUSY;
		goto out;
	}

	/* Save a pointer to the kernel part of the task struct. */
	uvfb_tasks[seq] = task;
	mutex_unlock(&uvfb_lock);

	err = cn_netlink_send(m, 0, 0, GFP_KERNEL);
	if (err == -ESRCH) {
		/*
		 * Try to start the userspace helper if sending
		 * the request failed the first time.
		 */
		err = uvesafb_helper_start();
		if (err) {
			pr_err("failed to execute %s\n", v86d_path);
			pr_err("make sure that the v86d helper is installed and executable\n");
		} else {
			v86d_started = 1;
			err = cn_netlink_send(m, 0, 0, gfp_any());
			if (err == -ENOBUFS)
				err = 0;
		}
	} else if (err == -ENOBUFS)
		err = 0;

	if (!err && !(task->t.flags & TF_EXIT))
		err = !wait_for_completion_timeout(task->done,
				msecs_to_jiffies(UVESAFB_TIMEOUT));

	mutex_lock(&uvfb_lock);
	uvfb_tasks[seq] = NULL;
	mutex_unlock(&uvfb_lock);

	seq++;
	if (seq >= UVESAFB_TASKS_MAX)
		seq = 0;
out:
	kfree(m);
	return err;
}