bool Item_subselect::update_table_bitmaps_processor(void *arg)
{
  List_iterator<Ref_to_outside> it(upper_refs);
  Ref_to_outside *upper;

  while ((upper= it++))
  {
    if (upper->item &&
        upper->item->walk(&Item::update_table_bitmaps_processor, FALSE, arg))
      return TRUE;
  }
  return FALSE;
}