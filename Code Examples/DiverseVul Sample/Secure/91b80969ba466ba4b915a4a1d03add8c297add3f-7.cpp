static int find_uid(struct posix_acl_state *state, struct posix_ace_state_array *a, uid_t uid)
{
	int i;

	for (i = 0; i < a->n; i++)
		if (a->aces[i].uid == uid)
			return i;
	/* Not found: */
	a->n++;
	a->aces[i].uid = uid;
	a->aces[i].perms.allow = state->everyone.allow;
	a->aces[i].perms.deny  = state->everyone.deny;

	return i;
}