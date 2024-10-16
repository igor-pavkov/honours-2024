static int keyring_match(const struct key *keyring, const void *description)
{
	return keyring->description &&
		strcmp(keyring->description, description) == 0;

} /* end keyring_match() */