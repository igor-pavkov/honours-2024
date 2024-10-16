static struct Qdisc *qdisc_leaf(struct Qdisc *p, u32 classid)
{
	unsigned long cl;
	struct Qdisc *leaf;
	const struct Qdisc_class_ops *cops = p->ops->cl_ops;

	if (cops == NULL)
		return NULL;
	cl = cops->get(p, classid);

	if (cl == 0)
		return NULL;
	leaf = cops->leaf(p, cl);
	cops->put(p, cl);
	return leaf;
}