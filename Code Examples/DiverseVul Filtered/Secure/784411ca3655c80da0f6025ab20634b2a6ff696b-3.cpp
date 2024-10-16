static void dumpL(field_t * info, int level)
{
    int i;

    indent(level);
    if (info->n_flds == 0) {
	fprintf(stderr, "Label \"%s\" ", info->lp->text);
	prbox(info->b);
    } else {
	fprintf(stderr, "Tbl ");
	prbox(info->b);
	for (i = 0; i < info->n_flds; i++) {
	    dumpL(info->fld[i], level + 1);
	}
    }
}