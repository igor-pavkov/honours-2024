struct key *key_lookup(key_serial_t id)
{
	struct rb_node *n;
	struct key *key;

	spin_lock(&key_serial_lock);

	/* search the tree for the specified key */
	n = key_serial_tree.rb_node;
	while (n) {
		key = rb_entry(n, struct key, serial_node);

		if (id < key->serial)
			n = n->rb_left;
		else if (id > key->serial)
			n = n->rb_right;
		else
			goto found;
	}

not_found:
	key = ERR_PTR(-ENOKEY);
	goto error;

found:
	/* A key is allowed to be looked up only if someone still owns a
	 * reference to it - otherwise it's awaiting the gc.
	 */
	if (!refcount_inc_not_zero(&key->usage))
		goto not_found;

error:
	spin_unlock(&key_serial_lock);
	return key;
}