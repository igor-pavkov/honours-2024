void init_hash_table(struct io_hash_table *table, unsigned size)
{
	unsigned int i;

	for (i = 0; i < size; i++) {
		spin_lock_init(&table->hbs[i].lock);
		INIT_HLIST_HEAD(&table->hbs[i].list);
	}
}