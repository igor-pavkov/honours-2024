void key_put(struct key *key)
{
	if (key) {
		key_check(key);

		if (refcount_dec_and_test(&key->usage))
			schedule_work(&key_gc_work);
	}
}