int key_link(struct key *keyring, struct key *key)
{
	int ret;

	key_check(keyring);
	key_check(key);

	down_write(&keyring->sem);
	ret = __key_link(keyring, key);
	up_write(&keyring->sem);

	return ret;

} /* end key_link() */