int unregister_qdisc(struct Qdisc_ops *qops)
{
	struct Qdisc_ops *q, **qp;
	int err = -ENOENT;

	write_lock(&qdisc_mod_lock);
	for (qp = &qdisc_base; (q=*qp)!=NULL; qp = &q->next)
		if (q == qops)
			break;
	if (q) {
		*qp = q->next;
		q->next = NULL;
		err = 0;
	}
	write_unlock(&qdisc_mod_lock);
	return err;
}