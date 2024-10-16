bool Ordered_key::init(MY_BITMAP *columns_to_index)
{
  THD *thd= tbl->in_use;
  uint cur_key_col= 0;
  Item_field *cur_tmp_field;
  Item_func_lt *fn_less_than;

  key_column_count= bitmap_bits_set(columns_to_index);
  key_columns= (Item_field**) thd->alloc(key_column_count *
                                         sizeof(Item_field*));
  compare_pred= (Item_func_lt**) thd->alloc(key_column_count *
                                            sizeof(Item_func_lt*));

  if (!key_columns || !compare_pred)
    return TRUE; /* Revert to table scan partial match. */

  for (uint i= 0; i < columns_to_index->n_bits; i++)
  {
    if (!bitmap_is_set(columns_to_index, i))
      continue;
    cur_tmp_field= new (thd->mem_root) Item_field(thd, tbl->field[i]);
    /* Create the predicate (tmp_column[i] < outer_ref[i]). */
    fn_less_than= new (thd->mem_root) Item_func_lt(thd, cur_tmp_field,
                                   search_key->element_index(i));
    fn_less_than->fix_fields(thd, (Item**) &fn_less_than);
    key_columns[cur_key_col]= cur_tmp_field;
    compare_pred[cur_key_col]= fn_less_than;
    ++cur_key_col;
  }

  if (alloc_keys_buffers())
  {
    /* TIMOUR revert to partial match via table scan. */
    return TRUE;
  }
  return FALSE;
}