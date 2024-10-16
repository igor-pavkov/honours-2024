static bool checkreturn pb_field_find(pb_field_iterator_t *iter, uint32_t tag)
{
    unsigned start = iter->field_index;
    
    do {
        if (iter->pos->tag == tag &&
            PB_LTYPE(iter->pos->type) != PB_LTYPE_EXTENSION)
        {
            return true;
        }
        (void)pb_field_next(iter);
    } while (iter->field_index != start);
    
    return false;
}