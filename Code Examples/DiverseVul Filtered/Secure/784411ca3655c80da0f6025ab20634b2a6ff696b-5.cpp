static void free_field(field_t * f)
{
    int i;

    for (i = 0; i < f->n_flds; i++) {
	free_field(f->fld[i]);
    }

    free(f->id);
    free_label(f->lp);
    free(f->fld);
    free(f);
}