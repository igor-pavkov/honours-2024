static void initialize_pointer_field(void *pItem, pb_field_iter_t *field)
{
    if (PB_LTYPE(field->type) == PB_LTYPE_STRING ||
        PB_LTYPE(field->type) == PB_LTYPE_BYTES)
    {
        *(void**)pItem = NULL;
    }
    else if (PB_LTYPE_IS_SUBMSG(field->type))
    {
        /* We memset to zero so that any callbacks are set to NULL.
         * Default values will be set by pb_dec_submessage(). */
        memset(pItem, 0, field->data_size);
    }
}