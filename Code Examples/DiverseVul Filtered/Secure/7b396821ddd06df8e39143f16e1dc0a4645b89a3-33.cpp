static void pb_message_set_to_defaults(const pb_field_t fields[], void *dest_struct)
{
    pb_field_iterator_t iter;
    pb_field_init(&iter, fields, dest_struct);
    
    do
    {
        pb_type_t type;
        type = iter.pos->type;
    
        /* Avoid crash on empty message types (zero fields) */
        if (iter.pos->tag == 0)
            continue;
        
        if (PB_ATYPE(type) == PB_ATYPE_STATIC)
        {
            if (PB_HTYPE(type) == PB_HTYPE_OPTIONAL)
            {
                /* Set has_field to false. Still initialize the optional field
                 * itself also. */
                *(bool*)iter.pSize = false;
            }
            else if (PB_HTYPE(type) == PB_HTYPE_REPEATED)
            {
                /* Set array count to 0, no need to initialize contents. */
                *(size_t*)iter.pSize = 0;
                continue;
            }
            
            if (PB_LTYPE(iter.pos->type) == PB_LTYPE_SUBMESSAGE)
            {
                /* Initialize submessage to defaults */
                pb_message_set_to_defaults((const pb_field_t *) iter.pos->ptr, iter.pData);
            }
            else if (iter.pos->ptr != NULL)
            {
                /* Initialize to default value */
                memcpy(iter.pData, iter.pos->ptr, iter.pos->data_size);
            }
            else
            {
                /* Initialize to zeros */
                memset(iter.pData, 0, iter.pos->data_size);
            }
        }
        else if (PB_ATYPE(type) == PB_ATYPE_POINTER)
        {
            /* Initialize the pointer to NULL. */
            *(void**)iter.pData = NULL;
            
            /* Initialize array count to 0. */
            if (PB_HTYPE(type) == PB_HTYPE_REPEATED)
            {
                *(size_t*)iter.pSize = 0;
            }
        }
        else if (PB_ATYPE(type) == PB_ATYPE_CALLBACK)
        {
            /* Don't overwrite callback */
        }
    } while (pb_field_next(&iter));
}