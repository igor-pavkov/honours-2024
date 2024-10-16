static void free_extended_entry(struct tree_desc_x *t)
{
	struct tree_desc_skip *p, *s;

	for (s = t->skip; s; s = p) {
		p = s->prev;
		free(s);
	}
}