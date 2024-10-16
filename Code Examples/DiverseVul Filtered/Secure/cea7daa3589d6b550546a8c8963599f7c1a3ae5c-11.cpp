static long keyring_read(const struct key *keyring,
			 char __user *buffer, size_t buflen)
{
	struct keyring_list *klist;
	struct key *key;
	size_t qty, tmp;
	int loop, ret;

	ret = 0;
	klist = keyring->payload.subscriptions;

	if (klist) {
		/* calculate how much data we could return */
		qty = klist->nkeys * sizeof(key_serial_t);

		if (buffer && buflen > 0) {
			if (buflen > qty)
				buflen = qty;

			/* copy the IDs of the subscribed keys into the
			 * buffer */
			ret = -EFAULT;

			for (loop = 0; loop < klist->nkeys; loop++) {
				key = klist->keys[loop];

				tmp = sizeof(key_serial_t);
				if (tmp > buflen)
					tmp = buflen;

				if (copy_to_user(buffer,
						 &key->serial,
						 tmp) != 0)
					goto error;

				buflen -= tmp;
				if (buflen == 0)
					break;
				buffer += tmp;
			}
		}

		ret = qty;
	}

 error:
	return ret;

} /* end keyring_read() */