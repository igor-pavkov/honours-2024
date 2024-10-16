int auth_policy_strptrcmp(const char *a0, const char *a1,
			  const char *b0, const char *b1)
{
	i_assert(a0 <= a1 && b0 <= b1);
	return memcmp(a0, b0, I_MIN((a1-a0),(b1-b0)));
}