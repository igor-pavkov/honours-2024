static void alias_grab(
	/*! [out] XML alias object. */
	struct xml_alias_t *alias)
{
	ithread_mutex_lock(&gWebMutex);
	assert(is_valid_alias(&gAliasDoc));
	memcpy(alias, &gAliasDoc, sizeof(struct xml_alias_t));
	*alias->ct = *alias->ct + 1;
	ithread_mutex_unlock(&gWebMutex);
}