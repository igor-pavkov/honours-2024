static bool key_is_dead(struct key *key, time_t limit)
{
	return test_bit(KEY_FLAG_DEAD, &key->flags) ||
		(key->expiry > 0 && key->expiry <= limit);
}