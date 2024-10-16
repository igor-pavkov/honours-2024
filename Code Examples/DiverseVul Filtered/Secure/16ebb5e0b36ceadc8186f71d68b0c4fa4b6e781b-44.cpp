static struct Qdisc *qdisc_match_from_root(struct Qdisc *root, u32 handle)
{
	struct Qdisc *q;

	if (!(root->flags & TCQ_F_BUILTIN) &&
	    root->handle == handle)
		return root;

	list_for_each_entry(q, &root->list, list) {
		if (q->handle == handle)
			return q;
	}
	return NULL;
}