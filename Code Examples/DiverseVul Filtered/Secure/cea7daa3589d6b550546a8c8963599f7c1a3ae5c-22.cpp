void keyring_gc(struct key *keyring, time_t limit)
{
	struct keyring_list *klist, *new;
	struct key *key;
	int loop, keep, max;

	kenter("{%x,%s}", key_serial(keyring), keyring->description);

	down_write(&keyring->sem);

	klist = keyring->payload.subscriptions;
	if (!klist)
		goto no_klist;

	/* work out how many subscriptions we're keeping */
	keep = 0;
	for (loop = klist->nkeys - 1; loop >= 0; loop--)
		if (!key_is_dead(klist->keys[loop], limit))
			keep++;

	if (keep == klist->nkeys)
		goto just_return;

	/* allocate a new keyring payload */
	max = roundup(keep, 4);
	new = kmalloc(sizeof(struct keyring_list) + max * sizeof(struct key *),
		      GFP_KERNEL);
	if (!new)
		goto nomem;
	new->maxkeys = max;
	new->nkeys = 0;
	new->delkey = 0;

	/* install the live keys
	 * - must take care as expired keys may be updated back to life
	 */
	keep = 0;
	for (loop = klist->nkeys - 1; loop >= 0; loop--) {
		key = klist->keys[loop];
		if (!key_is_dead(key, limit)) {
			if (keep >= max)
				goto discard_new;
			new->keys[keep++] = key_get(key);
		}
	}
	new->nkeys = keep;

	/* adjust the quota */
	key_payload_reserve(keyring,
			    sizeof(struct keyring_list) +
			    KEYQUOTA_LINK_BYTES * keep);

	if (keep == 0) {
		rcu_assign_pointer(keyring->payload.subscriptions, NULL);
		kfree(new);
	} else {
		rcu_assign_pointer(keyring->payload.subscriptions, new);
	}

	up_write(&keyring->sem);

	call_rcu(&klist->rcu, keyring_clear_rcu_disposal);
	kleave(" [yes]");
	return;

discard_new:
	new->nkeys = keep;
	keyring_clear_rcu_disposal(&new->rcu);
	up_write(&keyring->sem);
	kleave(" [discard]");
	return;

just_return:
	up_write(&keyring->sem);
	kleave(" [no dead]");
	return;

no_klist:
	up_write(&keyring->sem);
	kleave(" [no_klist]");
	return;

nomem:
	up_write(&keyring->sem);
	kleave(" [oom]");
}