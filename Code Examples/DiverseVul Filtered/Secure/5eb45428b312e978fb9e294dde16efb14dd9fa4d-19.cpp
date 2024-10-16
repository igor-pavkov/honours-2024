static struct alloted_s *append_alloted(struct alloted_s **head, char *name,
					int n)
{
	struct alloted_s *cur, *al;

	if (!head || !name) {
		/* Sanity check. Parameters should not be null. */
		usernic_error("%s\n", "Unexpected NULL argument");
		return NULL;
	}

	al = malloc(sizeof(struct alloted_s));
	if (!al) {
		usernic_error("Failed to allocate memory: %s\n",
			      strerror(errno));
		return NULL;
	}

	al->name = strdup(name);
	if (!al->name) {
		free(al);
		return NULL;
	}

	al->allowed = n;
	al->next = NULL;

	if (!*head) {
		*head = al;
		return al;
	}

	cur = *head;
	while (cur->next)
		cur = cur->next;
	cur->next = al;

	return al;
}