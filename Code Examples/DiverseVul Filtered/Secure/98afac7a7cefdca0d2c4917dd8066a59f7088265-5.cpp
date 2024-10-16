static void module_list_active(struct module_list *list)
{
	int i;
	struct module_list active_modules = MODULE_LIST_INIT;

	gitmodules_config();

	for (i = 0; i < list->nr; i++) {
		const struct cache_entry *ce = list->entries[i];

		if (!is_submodule_active(the_repository, ce->name))
			continue;

		ALLOC_GROW(active_modules.entries,
			   active_modules.nr + 1,
			   active_modules.alloc);
		active_modules.entries[active_modules.nr++] = ce;
	}

	free(list->entries);
	*list = active_modules;
}