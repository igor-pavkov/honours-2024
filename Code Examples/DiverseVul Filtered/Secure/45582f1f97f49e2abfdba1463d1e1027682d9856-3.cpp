static bool checkreturn find_extension_field(pb_field_iter_t *iter)
{
    pb_size_t start = iter->index;

    do {
        if (PB_LTYPE(iter->type) == PB_LTYPE_EXTENSION)
            return true;
        (void)pb_field_iter_next(iter);
    } while (iter->index != start);
    
    return false;
}