key_ref_t __keyring_search_one(key_ref_t keyring_ref,
			       const struct key_type *ktype,
			       const char *description,
			       key_perm_t perm)
{
	struct keyring_list *klist;
	unsigned long possessed;
	struct key *keyring, *key;
	int loop;

	keyring = key_ref_to_ptr(keyring_ref);
	possessed = is_key_possessed(keyring_ref);

	rcu_read_lock();

	klist = rcu_dereference(keyring->payload.subscriptions);
	if (klist) {
		for (loop = 0; loop < klist->nkeys; loop++) {
			key = klist->keys[loop];

			if (key->type == ktype &&
			    (!key->type->match ||
			     key->type->match(key, description)) &&
			    key_permission(make_key_ref(key, possessed),
					   perm) == 0 &&
			    !test_bit(KEY_FLAG_REVOKED, &key->flags)
			    )
				goto found;
		}
	}

	rcu_read_unlock();
	return ERR_PTR(-ENOKEY);

 found:
	atomic_inc(&key->usage);
	rcu_read_unlock();
	return make_key_ref(key, possessed);

} /* end __keyring_search_one() */