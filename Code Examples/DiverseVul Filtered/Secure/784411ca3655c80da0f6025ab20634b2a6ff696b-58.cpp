static field_t *map_rec_port(field_t * f, char *str)
{
    field_t *rv;
    int sub;

    if (f->id && (streq(f->id, str)))
	rv = f;
    else {
	rv = NULL;
	for (sub = 0; sub < f->n_flds; sub++)
	    if ((rv = map_rec_port(f->fld[sub], str)))
		break;
    }
    return rv;
}