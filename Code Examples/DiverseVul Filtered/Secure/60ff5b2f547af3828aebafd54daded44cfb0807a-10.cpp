static void mark_key_instantiated(struct key *key, int reject_error)
{
	/* Commit the payload before setting the state; barrier versus
	 * key_read_state().
	 */
	smp_store_release(&key->state,
			  (reject_error < 0) ? reject_error : KEY_IS_POSITIVE);
}