static struct trie *make_trie_node(const char *key, void *value)
{
	struct trie *new_node = xcalloc(1, sizeof(*new_node));
	new_node->len = strlen(key);
	if (new_node->len) {
		new_node->contents = xmalloc(new_node->len);
		memcpy(new_node->contents, key, new_node->len);
	}
	new_node->value = value;
	return new_node;
}