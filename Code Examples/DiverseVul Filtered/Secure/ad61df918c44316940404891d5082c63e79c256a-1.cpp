static struct tcf_proto_ops *tcf_proto_lookup_ops(struct nlattr *kind)
{
	struct tcf_proto_ops *t = NULL;

	if (kind) {
		read_lock(&cls_mod_lock);
		for (t = tcf_proto_base; t; t = t->next) {
			if (nla_strcmp(kind, t->kind) == 0) {
				if (!try_module_get(t->owner))
					t = NULL;
				break;
			}
		}
		read_unlock(&cls_mod_lock);
	}
	return t;
}