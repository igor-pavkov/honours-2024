static int ebt_compat_match_offset(const struct xt_match *match,
				   unsigned int userlen)
{
	/*
	 * ebt_among needs special handling. The kernel .matchsize is
	 * set to -1 at registration time; at runtime an EBT_ALIGN()ed
	 * value is expected.
	 * Example: userspace sends 4500, ebt_among.c wants 4504.
	 */
	if (unlikely(match->matchsize == -1))
		return XT_ALIGN(userlen) - COMPAT_XT_ALIGN(userlen);
	return xt_compat_match_offset(match);
}