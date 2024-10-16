void dm_deferred_remove(void)
{
	dm_hash_remove_all(true, false, true);
}