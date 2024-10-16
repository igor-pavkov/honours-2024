int auth_policy_attribute_comparator(const struct policy_template_keyvalue *a,
	const struct policy_template_keyvalue *b)
{
	return strcmp(a->key, b->key);
}