int subselect_indexsubquery_engine::exec()
{
  DBUG_ENTER("subselect_indexsubquery_engine");
  int error;
  bool null_finding= 0;
  TABLE *table= tab->table;
  Item_in_subselect *in_subs= (Item_in_subselect *) item;

  ((Item_in_subselect *) item)->value= 0;
  empty_result_set= TRUE;
  table->status= 0;

  if (check_null)
  {
    /* We need to check for NULL if there wasn't a matching value */
    *tab->ref.null_ref_key= 0;			// Search first for not null
    ((Item_in_subselect *) item)->was_null= 0;
  }

  if (!tab->preread_init_done && tab->preread_init())
    DBUG_RETURN(1);

  if (in_subs->left_expr_has_null())
  {
    /*
      The case when all values in left_expr are NULL is handled by
      Item_in_optimizer::val_int().
    */
    if (in_subs->is_top_level_item())
      DBUG_RETURN(1); /* notify caller to call reset() and set NULL value. */
    else
      DBUG_RETURN(scan_table());
  }

  if (copy_ref_key(true))
  {
    /* We know that there will be no rows even if we scan. */
    in_subs->value= 0;
    DBUG_RETURN(0);
  }

  if (!table->file->inited &&
      (error= table->file->ha_index_init(tab->ref.key, 1)))
  {
    (void) report_error(table, error);
    DBUG_RETURN(true);
  }

  error= table->file->ha_index_read_map(table->record[0],
                                        tab->ref.key_buff,
                                        make_prev_keypart_map(tab->
                                                              ref.key_parts),
                                        HA_READ_KEY_EXACT);
  if (error &&
      error != HA_ERR_KEY_NOT_FOUND && error != HA_ERR_END_OF_FILE)
    error= report_error(table, error);
  else
  {
    for (;;)
    {
      error= 0;
      table->null_row= 0;
      if (!table->status)
      {
        if ((!cond || cond->val_int()) && (!having || having->val_int()))
        {
          empty_result_set= FALSE;
          if (null_finding)
            ((Item_in_subselect *) item)->was_null= 1;
          else
            ((Item_in_subselect *) item)->value= 1;
          break;
        }
        error= table->file->ha_index_next_same(table->record[0],
                                               tab->ref.key_buff,
                                               tab->ref.key_length);
        if (error && error != HA_ERR_END_OF_FILE)
        {
          error= report_error(table, error);
          break;
        }
      }
      else
      {
        if (!check_null || null_finding)
          break;			/* We don't need to check nulls */
        *tab->ref.null_ref_key= 1;
        null_finding= 1;
        /* Check if there exists a row with a null value in the index */
        if ((error= (safe_index_read(tab) == 1)))
          break;
      }
    }
  }
  DBUG_RETURN(error != 0);
}