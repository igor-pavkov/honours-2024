long keyctl_set_timeout(key_serial_t id, unsigned timeout)
{
	struct timespec now;
	struct key *key;
	key_ref_t key_ref;
	time_t expiry;
	long ret;

	key_ref = lookup_user_key(id, 1, 1, KEY_SETATTR);
	if (IS_ERR(key_ref)) {
		ret = PTR_ERR(key_ref);
		goto error;
	}

	key = key_ref_to_ptr(key_ref);

	/* make the changes with the locks held to prevent races */
	down_write(&key->sem);

	expiry = 0;
	if (timeout > 0) {
		now = current_kernel_time();
		expiry = now.tv_sec + timeout;
	}

	key->expiry = expiry;

	up_write(&key->sem);
	key_put(key);

	ret = 0;
error:
	return ret;

} /* end keyctl_set_timeout() */