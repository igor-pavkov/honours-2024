static void initialize_pointer_field(void *pItem, pb_field_iterator_t *iter)
{
    if (PB_LTYPE(iter->pos->type) == PB_LTYPE_STRING ||
        PB_LTYPE(iter->pos->type) == PB_LTYPE_BYTES)
    {
        *(void**)pItem = NULL;
    }
    else if (PB_LTYPE(iter->pos->type) == PB_LTYPE_SUBMESSAGE)
    {
        pb_message_set_to_defaults((const pb_field_t *) iter->pos->ptr, pItem);
    }
}