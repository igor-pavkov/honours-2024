void qdisc_class_hash_insert(struct Qdisc_class_hash *clhash,
			     struct Qdisc_class_common *cl)
{
	unsigned int h;

	INIT_HLIST_NODE(&cl->hnode);
	h = qdisc_class_hash(cl->classid, clhash->hashmask);
	hlist_add_head(&cl->hnode, &clhash->hash[h]);
	clhash->hashelems++;
}