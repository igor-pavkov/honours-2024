static void iter_from_extension(pb_field_iterator_t *iter, pb_extension_t *extension)
{
    const pb_field_t *field = (const pb_field_t*)extension->type->arg;
    
    iter->start = field;
    iter->pos = field;
    iter->field_index = 0;
    iter->required_field_index = 0;
    iter->dest_struct = extension->dest;
    iter->pData = extension->dest;
    iter->pSize = &extension->found;
}