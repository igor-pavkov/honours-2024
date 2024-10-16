static UPNP_INLINE int is_valid_alias(
	/*! [in] XML alias object. */
	const struct xml_alias_t *alias)
{
	return alias->doc.buf != NULL;
}