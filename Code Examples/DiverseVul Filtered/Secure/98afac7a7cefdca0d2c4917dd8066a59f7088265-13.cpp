static int add_possible_reference_from_superproject(
		struct alternate_object_database *alt, void *sas_cb)
{
	struct submodule_alternate_setup *sas = sas_cb;

	/*
	 * If the alternate object store is another repository, try the
	 * standard layout with .git/(modules/<name>)+/objects
	 */
	if (ends_with(alt->path, "/objects")) {
		char *sm_alternate;
		struct strbuf sb = STRBUF_INIT;
		struct strbuf err = STRBUF_INIT;
		strbuf_add(&sb, alt->path, strlen(alt->path) - strlen("objects"));

		/*
		 * We need to end the new path with '/' to mark it as a dir,
		 * otherwise a submodule name containing '/' will be broken
		 * as the last part of a missing submodule reference would
		 * be taken as a file name.
		 */
		strbuf_addf(&sb, "modules/%s/", sas->submodule_name);

		sm_alternate = compute_alternate_path(sb.buf, &err);
		if (sm_alternate) {
			string_list_append(sas->reference, xstrdup(sb.buf));
			free(sm_alternate);
		} else {
			switch (sas->error_mode) {
			case SUBMODULE_ALTERNATE_ERROR_DIE:
				die(_("submodule '%s' cannot add alternate: %s"),
				    sas->submodule_name, err.buf);
			case SUBMODULE_ALTERNATE_ERROR_INFO:
				fprintf(stderr, _("submodule '%s' cannot add alternate: %s"),
					sas->submodule_name, err.buf);
			case SUBMODULE_ALTERNATE_ERROR_IGNORE:
				; /* nothing */
			}
		}
		strbuf_release(&sb);
	}

	return 0;
}