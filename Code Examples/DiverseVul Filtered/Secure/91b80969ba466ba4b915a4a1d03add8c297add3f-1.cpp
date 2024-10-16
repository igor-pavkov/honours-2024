static inline void add_to_mask(struct posix_acl_state *state, struct posix_ace_state *astate)
{
	state->mask.allow |= astate->allow;
}