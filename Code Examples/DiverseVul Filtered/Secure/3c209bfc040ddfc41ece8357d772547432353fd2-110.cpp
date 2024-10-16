int subselect_uniquesubquery_engine::index_lookup()
{
  DBUG_ENTER("subselect_uniquesubquery_engine::index_lookup");
  int error;
  TABLE *table= tab->table;
 
  if (!table->file->inited)
    table->file->ha_index_init(tab->ref.key, 0);
  error= table->file->ha_index_read_map(table->record[0],
                                        tab->ref.key_buff,
                                        make_prev_keypart_map(tab->
                                                              ref.key_parts),
                                        HA_READ_KEY_EXACT);
  DBUG_PRINT("info", ("lookup result: %i", error));

  if (error && error != HA_ERR_KEY_NOT_FOUND && error != HA_ERR_END_OF_FILE)
  {
    /*
      TIMOUR: I don't understand at all when do we need to call report_error.
      In most places where we access an index, we don't do this. Why here?
    */
    error= report_error(table, error);
    DBUG_RETURN(error);
  }

  table->null_row= 0;
  if (!error && (!cond || cond->val_int()))
    ((Item_in_subselect *) item)->value= 1;
  else
    ((Item_in_subselect *) item)->value= 0;

  DBUG_RETURN(0);
}