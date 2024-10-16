static int lookup_pi_state(u32 __user *uaddr, u32 uval,
			   struct futex_hash_bucket *hb,
			   union futex_key *key, struct futex_pi_state **ps)
{
	struct futex_q *top_waiter = futex_top_waiter(hb, key);

	/*
	 * If there is a waiter on that futex, validate it and
	 * attach to the pi_state when the validation succeeds.
	 */
	if (top_waiter)
		return attach_to_pi_state(uaddr, uval, top_waiter->pi_state, ps);

	/*
	 * We are the first waiter - try to look up the owner based on
	 * @uval and attach to it.
	 */
	return attach_to_pi_owner(uval, key, ps);
}