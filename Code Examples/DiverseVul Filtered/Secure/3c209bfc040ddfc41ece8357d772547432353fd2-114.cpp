bool Item_subselect::enumerate_field_refs_processor(void *arg)
{
  List_iterator<Ref_to_outside> it(upper_refs);
  Ref_to_outside *upper;
  
  while ((upper= it++))
  {
    if (upper->item &&
        upper->item->walk(&Item::enumerate_field_refs_processor, FALSE, arg))
      return TRUE;
  }
  return FALSE;
}