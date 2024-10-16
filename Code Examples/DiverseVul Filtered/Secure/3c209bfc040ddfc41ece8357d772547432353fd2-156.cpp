subselect_hash_sj_engine::get_strategy_using_data()
{
  Item_in_subselect *item_in= (Item_in_subselect *) item;
  select_materialize_with_stats *result_sink=
    (select_materialize_with_stats *) result;
  Item *outer_col;

  /*
    If we already determined that a complete match is enough based on schema
    information, nothing can be better.
  */
  if (strategy == COMPLETE_MATCH)
    return COMPLETE_MATCH;

  for (uint i= 0; i < item_in->left_expr->cols(); i++)
  {
    if (!bitmap_is_set(&partial_match_key_parts, i))
      continue;
    outer_col= item_in->left_expr->element_index(i);
    /*
      If column 'i' doesn't contain NULLs, and the corresponding outer reference
      cannot have a NULL value, then 'i' is a non-nullable column.
    */
    if (result_sink->get_null_count_of_col(i) == 0 && !outer_col->maybe_null)
    {
      bitmap_clear_bit(&partial_match_key_parts, i);
      bitmap_set_bit(&non_null_key_parts, i);
      --count_partial_match_columns;
    }
    if (result_sink->get_null_count_of_col(i) == tmp_table->file->stats.records)
      ++count_null_only_columns;
    if (result_sink->get_null_count_of_col(i))
      ++count_columns_with_nulls;
  }

  /* If no column contains NULLs use regular hash index lookups. */
  if (!count_partial_match_columns)
    return COMPLETE_MATCH;
  return PARTIAL_MATCH;
}