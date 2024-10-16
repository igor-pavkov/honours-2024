SETDEFAULTS_FUNC(mod_alias_set_defaults) {
	plugin_data *p = p_d;
	size_t i = 0;

	config_values_t cv[] = {
		{ "alias.url",                  NULL, T_CONFIG_ARRAY, T_CONFIG_SCOPE_CONNECTION },       /* 0 */
		{ NULL,                         NULL, T_CONFIG_UNSET,  T_CONFIG_SCOPE_UNSET }
	};

	if (!p) return HANDLER_ERROR;

	p->config_storage = calloc(1, srv->config_context->used * sizeof(plugin_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		data_config const* config = (data_config const*)srv->config_context->data[i];
		plugin_config *s;

		s = calloc(1, sizeof(plugin_config));
		s->alias = array_init();
		cv[0].destination = s->alias;

		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, config->value, cv, i == 0 ? T_CONFIG_SCOPE_SERVER : T_CONFIG_SCOPE_CONNECTION)) {
			return HANDLER_ERROR;
		}

		if (!array_is_kvstring(s->alias)) {
			log_error_write(srv, __FILE__, __LINE__, "s",
					"unexpected value for alias.url; expected list of \"urlpath\" => \"filepath\"");
			return HANDLER_ERROR;
		}

		if (s->alias->used >= 2) {
			const array *a = s->alias;
			size_t j, k;

			for (j = 0; j < a->used; j ++) {
				const buffer *prefix = a->data[a->sorted[j]]->key;
				for (k = j + 1; k < a->used; k ++) {
					const buffer *key = a->data[a->sorted[k]]->key;

					if (buffer_string_length(key) < buffer_string_length(prefix)) {
						break;
					}
					if (memcmp(key->ptr, prefix->ptr, buffer_string_length(prefix)) != 0) {
						break;
					}
					/* ok, they have same prefix. check position */
					if (a->sorted[j] < a->sorted[k]) {
						log_error_write(srv, __FILE__, __LINE__, "SBSBS",
							"url.alias: `", key, "' will never match as `", prefix, "' matched first");
						return HANDLER_ERROR;
					}
				}
			}
		}
	}

	return HANDLER_GO_ON;
}