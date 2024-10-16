int handler::index_next_same(uchar *buf, const uchar *key, uint keylen)
{
  int error;
  DBUG_ENTER("handler::index_next_same");
  if (!(error=index_next(buf)))
  {
    my_ptrdiff_t ptrdiff= buf - table->record[0];
    uchar *UNINIT_VAR(save_record_0);
    KEY *UNINIT_VAR(key_info);
    KEY_PART_INFO *UNINIT_VAR(key_part);
    KEY_PART_INFO *UNINIT_VAR(key_part_end);

    /*
      key_cmp_if_same() compares table->record[0] against 'key'.
      In parts it uses table->record[0] directly, in parts it uses
      field objects with their local pointers into table->record[0].
      If 'buf' is distinct from table->record[0], we need to move
      all record references. This is table->record[0] itself and
      the field pointers of the fields used in this key.
    */
    if (ptrdiff)
    {
      save_record_0= table->record[0];
      table->record[0]= buf;
      key_info= table->key_info + active_index;
      key_part= key_info->key_part;
      key_part_end= key_part + key_info->user_defined_key_parts;
      for (; key_part < key_part_end; key_part++)
      {
        DBUG_ASSERT(key_part->field);
        key_part->field->move_field_offset(ptrdiff);
      }
    }

    if (key_cmp_if_same(table, key, active_index, keylen))
    {
      table->status=STATUS_NOT_FOUND;
      error=HA_ERR_END_OF_FILE;
    }

    /* Move back if necessary. */
    if (ptrdiff)
    {
      table->record[0]= save_record_0;
      for (key_part= key_info->key_part; key_part < key_part_end; key_part++)
        key_part->field->move_field_offset(-ptrdiff);
    }
  }
  DBUG_PRINT("return",("%i", error));
  DBUG_RETURN(error);
}