static MXFIndexTable *mxf_find_index_table(MXFContext *mxf, int index_sid)
{
    int i;
    for (i = 0; i < mxf->nb_index_tables; i++)
        if (mxf->index_tables[i].index_sid == index_sid)
            return &mxf->index_tables[i];
    return NULL;
}