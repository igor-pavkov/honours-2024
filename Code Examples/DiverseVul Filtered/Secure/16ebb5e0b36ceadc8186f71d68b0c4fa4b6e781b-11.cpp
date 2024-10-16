void qdisc_tree_decrease_qlen(struct Qdisc *sch, unsigned int n)
{
	const struct Qdisc_class_ops *cops;
	unsigned long cl;
	u32 parentid;

	if (n == 0)
		return;
	while ((parentid = sch->parent)) {
		if (TC_H_MAJ(parentid) == TC_H_MAJ(TC_H_INGRESS))
			return;

		sch = qdisc_lookup(qdisc_dev(sch), TC_H_MAJ(parentid));
		if (sch == NULL) {
			WARN_ON(parentid != TC_H_ROOT);
			return;
		}
		cops = sch->ops->cl_ops;
		if (cops->qlen_notify) {
			cl = cops->get(sch, parentid);
			cops->qlen_notify(sch, cl);
			cops->put(sch, cl);
		}
		sch->q.qlen -= n;
	}
}