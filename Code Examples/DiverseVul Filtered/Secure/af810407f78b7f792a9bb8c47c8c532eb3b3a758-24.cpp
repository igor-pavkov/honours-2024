static int check_duplicate_long_entry_key(TABLE *table, handler *h,
                                          const uchar *new_rec, uint key_no)
{
  Field *hash_field;
  int result, error= 0;
  KEY *key_info= table->key_info + key_no;
  hash_field= key_info->key_part->field;
  uchar ptr[HA_HASH_KEY_LENGTH_WITH_NULL];

  DBUG_ASSERT((key_info->flags & HA_NULL_PART_KEY &&
               key_info->key_length == HA_HASH_KEY_LENGTH_WITH_NULL)
              || key_info->key_length == HA_HASH_KEY_LENGTH_WITHOUT_NULL);

  if (hash_field->is_real_null())
    return 0;

  key_copy(ptr, new_rec, key_info, key_info->key_length, false);

  if (!table->check_unique_buf)
    table->check_unique_buf= (uchar *)alloc_root(&table->mem_root,
                                                 table->s->reclength);

  result= h->ha_index_init(key_no, 0);
  if (result)
    return result;
  store_record(table, check_unique_buf);
  result= h->ha_index_read_map(table->record[0],
                               ptr, HA_WHOLE_KEY, HA_READ_KEY_EXACT);
  if (!result)
  {
    bool is_same;
    Field * t_field;
    Item_func_hash * temp= (Item_func_hash *)hash_field->vcol_info->expr;
    Item ** arguments= temp->arguments();
    uint arg_count= temp->argument_count();
    do
    {
      my_ptrdiff_t diff= table->check_unique_buf - new_rec;
      is_same= true;
      for (uint j=0; is_same && j < arg_count; j++)
      {
        DBUG_ASSERT(arguments[j]->type() == Item::FIELD_ITEM ||
                    // this one for left(fld_name,length)
                    arguments[j]->type() == Item::FUNC_ITEM);
        if (arguments[j]->type() == Item::FIELD_ITEM)
        {
          t_field= static_cast<Item_field *>(arguments[j])->field;
          if (t_field->cmp_offset(diff))
            is_same= false;
        }
        else
        {
          Item_func_left *fnc= static_cast<Item_func_left *>(arguments[j]);
          DBUG_ASSERT(!my_strcasecmp(system_charset_info, "left", fnc->func_name()));
          DBUG_ASSERT(fnc->arguments()[0]->type() == Item::FIELD_ITEM);
          t_field= static_cast<Item_field *>(fnc->arguments()[0])->field;
          uint length= (uint)fnc->arguments()[1]->val_int();
          if (t_field->cmp_prefix(t_field->ptr, t_field->ptr + diff, length))
            is_same= false;
        }
      }
    }
    while (!is_same && !(result= h->ha_index_next_same(table->record[0],
                         ptr, key_info->key_length)));
    if (is_same)
      error= HA_ERR_FOUND_DUPP_KEY;
    goto exit;
  }
  if (result != HA_ERR_KEY_NOT_FOUND)
    error= result;
exit:
  if (error == HA_ERR_FOUND_DUPP_KEY)
  {
    table->file->errkey= key_no;
    if (h->ha_table_flags() & HA_DUPLICATE_POS)
    {
      h->position(table->record[0]);
      memcpy(table->file->dup_ref, h->ref, h->ref_length);
    }
  }
  restore_record(table, check_unique_buf);
  h->ha_index_end();
  return error;
}