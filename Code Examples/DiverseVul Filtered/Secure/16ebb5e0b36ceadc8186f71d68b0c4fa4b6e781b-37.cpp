static struct Qdisc_ops *qdisc_lookup_ops(struct nlattr *kind)
{
	struct Qdisc_ops *q = NULL;

	if (kind) {
		read_lock(&qdisc_mod_lock);
		for (q = qdisc_base; q; q = q->next) {
			if (nla_strcmp(kind, q->id) == 0) {
				if (!try_module_get(q->owner))
					q = NULL;
				break;
			}
		}
		read_unlock(&qdisc_mod_lock);
	}
	return q;
}