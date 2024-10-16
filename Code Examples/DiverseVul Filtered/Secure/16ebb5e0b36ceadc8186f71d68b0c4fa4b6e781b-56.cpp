void qdisc_list_del(struct Qdisc *q)
{
	if ((q->parent != TC_H_ROOT) && !(q->flags & TCQ_F_INGRESS))
		list_del(&q->list);
}