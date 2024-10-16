subselect_hash_sj_engine::get_strategy_using_schema()
{
  Item_in_subselect *item_in= (Item_in_subselect *) item;

  if (item_in->is_top_level_item())
    return COMPLETE_MATCH;
  else
  {
    List_iterator<Item> inner_col_it(*item_in->unit->get_column_types(false));
    Item *outer_col, *inner_col;

    for (uint i= 0; i < item_in->left_expr->cols(); i++)
    {
      outer_col= item_in->left_expr->element_index(i);
      inner_col= inner_col_it++;

      if (!inner_col->maybe_null && !outer_col->maybe_null)
        bitmap_set_bit(&non_null_key_parts, i);
      else
      {
        bitmap_set_bit(&partial_match_key_parts, i);
        ++count_partial_match_columns;
      }
    }
  }

  /* If no column contains NULLs use regular hash index lookups. */
  if (count_partial_match_columns)
    return PARTIAL_MATCH;
  return COMPLETE_MATCH;
}