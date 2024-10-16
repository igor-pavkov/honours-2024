static bool checkreturn find_extension_field(pb_field_iterator_t *iter)
{
    unsigned start = iter->field_index;
    
    do {
        if (PB_LTYPE(iter->pos->type) == PB_LTYPE_EXTENSION)
            return true;
        (void)pb_field_next(iter);
    } while (iter->field_index != start);
    
    return false;
}