void qdisc_class_hash_destroy(struct Qdisc_class_hash *clhash)
{
	qdisc_class_hash_free(clhash->hash, clhash->hashsize);
}