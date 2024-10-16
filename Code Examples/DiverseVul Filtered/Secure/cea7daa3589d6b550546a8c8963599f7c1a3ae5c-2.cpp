static void keyring_revoke(struct key *keyring)
{
	struct keyring_list *klist = keyring->payload.subscriptions;

	/* adjust the quota */
	key_payload_reserve(keyring, 0);

	if (klist) {
		rcu_assign_pointer(keyring->payload.subscriptions, NULL);
		call_rcu(&klist->rcu, keyring_clear_rcu_disposal);
	}

} /* end keyring_revoke() */