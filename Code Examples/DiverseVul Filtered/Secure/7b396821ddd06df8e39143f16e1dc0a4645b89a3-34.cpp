static void pb_field_init(pb_field_iterator_t *iter, const pb_field_t *fields, void *dest_struct)
{
    iter->start = iter->pos = fields;
    iter->field_index = 0;
    iter->required_field_index = 0;
    iter->pData = (char*)dest_struct + iter->pos->data_offset;
    iter->pSize = (char*)iter->pData + iter->pos->size_offset;
    iter->dest_struct = dest_struct;
}