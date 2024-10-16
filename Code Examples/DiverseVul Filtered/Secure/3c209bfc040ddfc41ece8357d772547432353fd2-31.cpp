bool subselect_single_select_engine::fix_length_and_dec(Item_cache **row)
{
  DBUG_ASSERT(row || select_lex->item_list.elements==1);
  if (set_row(select_lex->item_list, row))
    return TRUE;
  item->collation.set(row[0]->collation);
  if (cols() != 1)
    maybe_null= 0;
  return FALSE;
}