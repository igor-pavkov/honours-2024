void qdisc_class_hash_grow(struct Qdisc *sch, struct Qdisc_class_hash *clhash)
{
	struct Qdisc_class_common *cl;
	struct hlist_node *n, *next;
	struct hlist_head *nhash, *ohash;
	unsigned int nsize, nmask, osize;
	unsigned int i, h;

	/* Rehash when load factor exceeds 0.75 */
	if (clhash->hashelems * 4 <= clhash->hashsize * 3)
		return;
	nsize = clhash->hashsize * 2;
	nmask = nsize - 1;
	nhash = qdisc_class_hash_alloc(nsize);
	if (nhash == NULL)
		return;

	ohash = clhash->hash;
	osize = clhash->hashsize;

	sch_tree_lock(sch);
	for (i = 0; i < osize; i++) {
		hlist_for_each_entry_safe(cl, n, next, &ohash[i], hnode) {
			h = qdisc_class_hash(cl->classid, nmask);
			hlist_add_head(&cl->hnode, &nhash[h]);
		}
	}
	clhash->hash     = nhash;
	clhash->hashsize = nsize;
	clhash->hashmask = nmask;
	sch_tree_unlock(sch);

	qdisc_class_hash_free(ohash, osize);
}