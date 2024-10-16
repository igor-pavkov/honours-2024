static void mf_add(mf_t *mf, const char *fname)
{
    char *entry = talloc_strdup(mf, fname);
    MP_TARRAY_APPEND(mf, mf->names, mf->nr_of_files, entry);
}