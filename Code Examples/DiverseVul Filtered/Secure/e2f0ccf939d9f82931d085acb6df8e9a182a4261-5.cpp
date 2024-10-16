void pb_release(const pb_msgdesc_t *fields, void *dest_struct)
{
    pb_field_iter_t iter;
    
    if (!dest_struct)
        return; /* Ignore NULL pointers, similar to free() */

    if (!pb_field_iter_begin(&iter, fields, dest_struct))
        return; /* Empty message type */
    
    do
    {
        pb_release_single_field(&iter);
    } while (pb_field_iter_next(&iter));
}