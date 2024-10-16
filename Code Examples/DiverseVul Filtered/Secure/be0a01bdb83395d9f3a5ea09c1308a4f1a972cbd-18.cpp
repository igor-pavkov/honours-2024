static void alias_release(
	/*! [in] XML alias object. */
	struct xml_alias_t *alias)
{
	ithread_mutex_lock(&gWebMutex);
	/* ignore invalid alias */
	if (!is_valid_alias(alias)) {
		ithread_mutex_unlock(&gWebMutex);
		return;
	}
	assert(*alias->ct > 0);
	*alias->ct -= 1;
	if (*alias->ct <= 0) {
		membuffer_destroy(&alias->doc);
		membuffer_destroy(&alias->name);
		free(alias->ct);
	}
	ithread_mutex_unlock(&gWebMutex);
}