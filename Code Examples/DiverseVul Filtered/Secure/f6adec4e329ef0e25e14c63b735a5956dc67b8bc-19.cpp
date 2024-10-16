static int name_and_item_from_var(const char *var, struct strbuf *name,
				  struct strbuf *item)
{
	const char *subsection, *key;
	int subsection_len, parse;
	parse = parse_config_key(var, "submodule", &subsection,
			&subsection_len, &key);
	if (parse < 0 || !subsection)
		return 0;

	strbuf_add(name, subsection, subsection_len);
	if (check_submodule_name(name->buf) < 0) {
		warning(_("ignoring suspicious submodule name: %s"), name->buf);
		strbuf_release(name);
		return 0;
	}

	strbuf_addstr(item, key);

	return 1;
}