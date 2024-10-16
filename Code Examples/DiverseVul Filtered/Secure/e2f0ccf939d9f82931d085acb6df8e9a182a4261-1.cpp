static bool pb_field_set_to_default(pb_field_iter_t *field)
{
    pb_type_t type;
    type = field->type;

    if (PB_LTYPE(type) == PB_LTYPE_EXTENSION)
    {
        pb_extension_t *ext = *(pb_extension_t* const *)field->pData;
        while (ext != NULL)
        {
            pb_field_iter_t ext_iter;
            if (pb_field_iter_begin_extension(&ext_iter, ext))
            {
                ext->found = false;
                if (!pb_message_set_to_defaults(&ext_iter))
                    return false;
            }
            ext = ext->next;
        }
    }
    else if (PB_ATYPE(type) == PB_ATYPE_STATIC)
    {
        bool init_data = true;
        if (PB_HTYPE(type) == PB_HTYPE_OPTIONAL && field->pSize != NULL)
        {
            /* Set has_field to false. Still initialize the optional field
             * itself also. */
            *(bool*)field->pSize = false;
        }
        else if (PB_HTYPE(type) == PB_HTYPE_REPEATED ||
                 PB_HTYPE(type) == PB_HTYPE_ONEOF)
        {
            /* REPEATED: Set array count to 0, no need to initialize contents.
               ONEOF: Set which_field to 0. */
            *(pb_size_t*)field->pSize = 0;
            init_data = false;
        }

        if (init_data)
        {
            if (PB_LTYPE_IS_SUBMSG(field->type) &&
                (field->submsg_desc->default_value != NULL ||
                 field->submsg_desc->field_callback != NULL ||
                 field->submsg_desc->submsg_info[0] != NULL))
            {
                /* Initialize submessage to defaults.
                 * Only needed if it has default values
                 * or callback/submessage fields. */
                pb_field_iter_t submsg_iter;
                if (pb_field_iter_begin(&submsg_iter, field->submsg_desc, field->pData))
                {
                    if (!pb_message_set_to_defaults(&submsg_iter))
                        return false;
                }
            }
            else
            {
                /* Initialize to zeros */
                memset(field->pData, 0, (size_t)field->data_size);
            }
        }
    }
    else if (PB_ATYPE(type) == PB_ATYPE_POINTER)
    {
        /* Initialize the pointer to NULL. */
        *(void**)field->pField = NULL;

        /* Initialize array count to 0. */
        if (PB_HTYPE(type) == PB_HTYPE_REPEATED ||
            PB_HTYPE(type) == PB_HTYPE_ONEOF)
        {
            *(pb_size_t*)field->pSize = 0;
        }
    }
    else if (PB_ATYPE(type) == PB_ATYPE_CALLBACK)
    {
        /* Don't overwrite callback */
    }

    return true;
}