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
         * Then set any default values. */
        pb_field_iter_t submsg_iter;
        memset(pItem, 0, field->data_size);

        if (pb_field_iter_begin(&submsg_iter, field->submsg_desc, pItem))
        {
            (void)pb_message_set_to_defaults(&submsg_iter);
        }
    }
}