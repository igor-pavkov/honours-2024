static void free_alloted(struct alloted_s **head)
{
	struct alloted_s *cur;

	if (!head)
		return;

	cur = *head;
	while (cur) {
		cur = cur->next;
		free((*head)->name);
		free(*head);
		*head = cur;
	}
}