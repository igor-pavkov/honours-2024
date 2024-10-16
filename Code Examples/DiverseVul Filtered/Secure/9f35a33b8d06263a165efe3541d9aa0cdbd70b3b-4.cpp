int user_match(const struct key *key, const void *description)
{
	return strcmp(key->description, description) == 0;
}