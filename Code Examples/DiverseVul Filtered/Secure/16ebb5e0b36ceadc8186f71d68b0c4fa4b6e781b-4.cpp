void qdisc_class_hash_remove(struct Qdisc_class_hash *clhash,
			     struct Qdisc_class_common *cl)
{
	hlist_del(&cl->hnode);
	clhash->hashelems--;
}