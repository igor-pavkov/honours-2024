int qdisc_class_hash_init(struct Qdisc_class_hash *clhash)
{
	unsigned int size = 4;

	clhash->hash = qdisc_class_hash_alloc(size);
	if (clhash->hash == NULL)
		return -ENOMEM;
	clhash->hashsize  = size;
	clhash->hashmask  = size - 1;
	clhash->hashelems = 0;
	return 0;
}