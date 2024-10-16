static void free_wildmats(struct wildmat *wild)
{
    struct wildmat *w = wild;

    while (w->pat) {
	free(w->pat);
	w++;
    }
    free(wild);
}