bool subselect_union_engine::fix_length_and_dec(Item_cache **row)
{
  DBUG_ASSERT(row || unit->first_select()->item_list.elements==1);

  if (unit->first_select()->item_list.elements == 1)
  {
    if (set_row(unit->types, row))
      return TRUE;
    item->collation.set(row[0]->collation);
  }
  else
  {
    bool maybe_null_saved= maybe_null;
    if (set_row(unit->types, row))
      return TRUE;
    maybe_null= maybe_null_saved;
  }
  return FALSE;
}