static inline int qdisc_qlen(struct Qdisc *q)
{
	return q->q.qlen;
}