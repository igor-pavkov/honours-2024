int subselect_uniquesubquery_engine::scan_table()
{
  int error;
  TABLE *table= tab->table;
  DBUG_ENTER("subselect_uniquesubquery_engine::scan_table");

  if ((table->file->inited &&
       (error= table->file->ha_index_end())) ||
      (error= table->file->ha_rnd_init(1)))
  {
    (void) report_error(table, error);
    DBUG_RETURN(true);
  }

  table->file->extra_opt(HA_EXTRA_CACHE,
                         get_thd()->variables.read_buff_size);
  table->null_row= 0;
  for (;;)
  {
    error=table->file->ha_rnd_next(table->record[0]);
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
        error= report_error(table, error);
        break;
      }
    }

    if (!cond || cond->val_int())
    {
      empty_result_set= FALSE;
      break;
    }
  }

  table->file->ha_rnd_end();
  DBUG_RETURN(error != 0);
}