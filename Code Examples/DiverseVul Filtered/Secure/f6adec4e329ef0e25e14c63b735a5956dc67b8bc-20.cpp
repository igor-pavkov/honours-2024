static unsigned int hash_sha1_string(const unsigned char *sha1,
				     const char *string)
{
	return memhash(sha1, 20) + strhash(string);
}