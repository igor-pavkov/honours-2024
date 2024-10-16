static UPNP_INLINE void glob_alias_init(void)
{
	struct xml_alias_t *alias = &gAliasDoc;

	membuffer_init(&alias->doc);
	membuffer_init(&alias->name);
	alias->ct = NULL;
	alias->last_modified = 0;
}