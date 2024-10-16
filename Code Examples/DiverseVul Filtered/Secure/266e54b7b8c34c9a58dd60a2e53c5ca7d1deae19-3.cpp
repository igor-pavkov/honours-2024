int imap_bodystructure_parse(const char *bodystructure,
	pool_t pool, struct message_part *parts,
	const char **error_r)
{
	i_assert(parts != NULL);

	return imap_bodystructure_parse_full(bodystructure,
		pool, &parts, error_r);
}