bool subselect_table_scan_engine::partial_match()
{
  List_iterator_fast<Item> equality_it(*equi_join_conds);
  Item *cur_eq;
  uint count_matches;
  int error;
  bool res;

  if (tmp_table->file->ha_rnd_init_with_error(1))
  {
    res= FALSE;
    goto end;
  }

  tmp_table->file->extra_opt(HA_EXTRA_CACHE,
                             get_thd()->variables.read_buff_size);
  for (;;)
  {
    error= tmp_table->file->ha_rnd_next(tmp_table->record[0]);
    if (error) {
      if (error == HA_ERR_RECORD_DELETED)
      {
        error= 0;
        continue;
      }
      if (error == HA_ERR_END_OF_FILE)
      {
        error= 0;
        break;
      }
      else
      {
        error= report_error(tmp_table, error);
        break;
      }
    }

    equality_it.rewind();
    count_matches= 0;
    while ((cur_eq= equality_it++))
    {
      DBUG_ASSERT(cur_eq->type() == Item::FUNC_ITEM &&
                  ((Item_func*)cur_eq)->functype() == Item_func::EQ_FUNC);
      if (!cur_eq->val_int() && !cur_eq->null_value)
        break;
      ++count_matches;
    }
    if (count_matches == tmp_table->s->fields)
    {
      res= TRUE; /* Found a matching row. */
      goto end;
    }
  }

  res= FALSE;
end:
  tmp_table->file->ha_rnd_end();
  return res;
}