static void free_one_config(struct submodule_entry *entry)
{
	free((void *) entry->config->path);
	free((void *) entry->config->name);
	free((void *) entry->config->branch);
	free((void *) entry->config->update_strategy.command);
	free(entry->config);
}