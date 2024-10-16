static void fib6_free_table(struct fib6_table *table)
{
	inetpeer_invalidate_tree(&table->tb6_peers);
	kfree(table);
}