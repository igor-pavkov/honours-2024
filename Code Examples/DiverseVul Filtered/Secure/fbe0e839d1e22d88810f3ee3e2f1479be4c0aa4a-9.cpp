static void get_pi_state(struct futex_pi_state *pi_state)
{
	WARN_ON_ONCE(!atomic_inc_not_zero(&pi_state->refcount));
}