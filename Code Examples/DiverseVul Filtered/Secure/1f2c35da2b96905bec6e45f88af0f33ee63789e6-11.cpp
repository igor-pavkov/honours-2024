void auth_policy_init(void)
{
	http_client_set.request_absolute_timeout_msecs = global_auth_settings->policy_server_timeout_msecs;
	if (global_auth_settings->debug)
		http_client_set.debug = 1;
	http_client = http_client_init(&http_client_set);

	/* prepare template */

	ARRAY(struct policy_template_keyvalue) attribute_pairs;
	const struct policy_template_keyvalue *kvptr;
	string_t *template = t_str_new(64);
	const char **ptr;
	const char *key = NULL;
	const char **list = t_strsplit_spaces(global_auth_settings->policy_request_attributes, "= ");

	t_array_init(&attribute_pairs, 8);
	for(ptr = list; *ptr != NULL; ptr++) {
		struct policy_template_keyvalue pair;
		if (key == NULL) {
			key = *ptr;
		} else {
			pair.key = key;
			pair.value = *ptr;
			key = NULL;
			array_append(&attribute_pairs, &pair, 1);
		}
	}
	if (key != NULL) {
		i_fatal("auth_policy_request_attributes contains invalid value");
	}

	/* then we sort it */
	array_sort(&attribute_pairs, auth_policy_attribute_comparator);

	/* and build a template string */
	const char *prevkey = "";

	array_foreach(&attribute_pairs, kvptr) {
		const char *kptr = strchr(kvptr->key, '/');
		auth_policy_open_and_close_to_key(prevkey, kvptr->key, template);
		str_append_c(template,'"');
		json_append_escaped(template, (kptr != NULL?kptr+1:kvptr->key));
		str_append_c(template,'"');
		str_append_c(template,':');
		str_append_c(template,'"');
		str_append(template,kvptr->value);
		str_append_c(template,'"');
		str_append_c(template,',');
		prevkey = kvptr->key;
	}

	auth_policy_open_and_close_to_key(prevkey, "", template);
	str_truncate(template, str_len(template)-1);
	auth_policy_json_template = i_strdup(str_c(template));
}