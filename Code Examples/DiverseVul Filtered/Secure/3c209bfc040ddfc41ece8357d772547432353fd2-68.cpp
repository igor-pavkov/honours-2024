bool subselect_single_select_engine::no_rows()
{ 
  return !item->assigned();
}