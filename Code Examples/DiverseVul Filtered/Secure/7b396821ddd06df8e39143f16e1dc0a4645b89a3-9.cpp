void pb_release(const pb_field_t fields[], void *dest_struct)
{
    pb_field_iterator_t iter;
    pb_field_init(&iter, fields, dest_struct);

    if (iter.pos->tag == 0)
        return; /* Empty message type */
    
    do
    {
        pb_release_single_field(&iter);
    } while (pb_field_next(&iter));
}