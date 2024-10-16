int register_qdisc(struct Qdisc_ops *qops)
{
	struct Qdisc_ops *q, **qp;
	int rc = -EEXIST;

	write_lock(&qdisc_mod_lock);
	for (qp = &qdisc_base; (q = *qp) != NULL; qp = &q->next)
		if (!strcmp(qops->id, q->id))
			goto out;

	if (qops->enqueue == NULL)
		qops->enqueue = noop_qdisc_ops.enqueue;
	if (qops->peek == NULL) {
		if (qops->dequeue == NULL) {
			qops->peek = noop_qdisc_ops.peek;
		} else {
			rc = -EINVAL;
			goto out;
		}
	}
	if (qops->dequeue == NULL)
		qops->dequeue = noop_qdisc_ops.dequeue;

	qops->next = NULL;
	*qp = qops;
	rc = 0;
out:
	write_unlock(&qdisc_mod_lock);
	return rc;
}