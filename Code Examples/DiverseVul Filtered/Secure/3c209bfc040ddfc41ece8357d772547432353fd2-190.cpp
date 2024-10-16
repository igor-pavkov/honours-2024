bool Ordered_key::init(int col_idx)
{
  THD *thd= tbl->in_use;

  key_column_count= 1;

  // TIMOUR: check for mem allocation err, revert to scan

  key_columns= (Item_field**) thd->alloc(sizeof(Item_field*));
  compare_pred= (Item_func_lt**) thd->alloc(sizeof(Item_func_lt*));

  key_columns[0]= new (thd->mem_root) Item_field(thd, tbl->field[col_idx]);
  /* Create the predicate (tmp_column[i] < outer_ref[i]). */
  compare_pred[0]= new (thd->mem_root) Item_func_lt(thd, key_columns[0],
                                    search_key->element_index(col_idx));
  compare_pred[0]->fix_fields(thd, (Item**)&compare_pred[0]);

  if (alloc_keys_buffers())
  {
    /* TIMOUR revert to partial match via table scan. */
    return TRUE;
  }
  return FALSE;
}