static int config_path_cmp(const void *unused_cmp_data,
			   const void *entry,
			   const void *entry_or_key,
			   const void *unused_keydata)
{
	const struct submodule_entry *a = entry;
	const struct submodule_entry *b = entry_or_key;

	return strcmp(a->config->path, b->config->path) ||
	       hashcmp(a->config->gitmodules_sha1, b->config->gitmodules_sha1);
}