static bool pb_field_next(pb_field_iterator_t *iter)
{
    bool notwrapped = true;
    size_t prev_size = iter->pos->data_size;
    
    if (PB_ATYPE(iter->pos->type) == PB_ATYPE_STATIC &&
        PB_HTYPE(iter->pos->type) == PB_HTYPE_REPEATED)
    {
        prev_size *= iter->pos->array_size;
    }
    else if (PB_ATYPE(iter->pos->type) == PB_ATYPE_POINTER)
    {
        prev_size = sizeof(void*);
    }
    
    if (iter->pos->tag == 0)
        return false; /* Only happens with empty message types */
    
    if (PB_HTYPE(iter->pos->type) == PB_HTYPE_REQUIRED)
        iter->required_field_index++;
    
    iter->pos++;
    iter->field_index++;
    if (iter->pos->tag == 0)
    {
        iter->pos = iter->start;
        iter->field_index = 0;
        iter->required_field_index = 0;
        iter->pData = iter->dest_struct;
        prev_size = 0;
        notwrapped = false;
    }
    
    iter->pData = (char*)iter->pData + prev_size + iter->pos->data_offset;
    iter->pSize = (char*)iter->pData + iter->pos->size_offset;
    return notwrapped;
}