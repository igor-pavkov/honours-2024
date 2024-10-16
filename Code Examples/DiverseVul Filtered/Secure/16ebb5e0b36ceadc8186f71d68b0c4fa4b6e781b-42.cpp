static void qdisc_list_add(struct Qdisc *q)
{
	if ((q->parent != TC_H_ROOT) && !(q->flags & TCQ_F_INGRESS))
		list_add_tail(&q->list, &qdisc_root_sleeping(q)->list);
}