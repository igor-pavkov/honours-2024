static void uvesafb_cn_callback(struct cn_msg *msg, struct netlink_skb_parms *nsp)
{
	struct uvesafb_task *utask;
	struct uvesafb_ktask *task;

	if (!capable(CAP_SYS_ADMIN))
		return;

	if (msg->seq >= UVESAFB_TASKS_MAX)
		return;

	mutex_lock(&uvfb_lock);
	task = uvfb_tasks[msg->seq];

	if (!task || msg->ack != task->ack) {
		mutex_unlock(&uvfb_lock);
		return;
	}

	utask = (struct uvesafb_task *)msg->data;

	/* Sanity checks for the buffer length. */
	if (task->t.buf_len < utask->buf_len ||
	    utask->buf_len > msg->len - sizeof(*utask)) {
		mutex_unlock(&uvfb_lock);
		return;
	}

	uvfb_tasks[msg->seq] = NULL;
	mutex_unlock(&uvfb_lock);

	memcpy(&task->t, utask, sizeof(*utask));

	if (task->t.buf_len && task->buf)
		memcpy(task->buf, utask + 1, task->t.buf_len);

	complete(task->done);
	return;
}