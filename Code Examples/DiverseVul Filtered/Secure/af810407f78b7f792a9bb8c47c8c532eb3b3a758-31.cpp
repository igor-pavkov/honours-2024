static int check_duplicate_long_entries(TABLE *table, handler *h,
                                        const uchar *new_rec)
{
  table->file->errkey= -1;
  int result;
  for (uint i= 0; i < table->s->keys; i++)
  {
    if (table->key_info[i].algorithm == HA_KEY_ALG_LONG_HASH &&
            (result= check_duplicate_long_entry_key(table, h, new_rec, i)))
      return result;
  }
  return 0;
}