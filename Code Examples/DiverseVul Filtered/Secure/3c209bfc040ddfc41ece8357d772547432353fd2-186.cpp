bool subselect_engine::set_row(List<Item> &item_list, Item_cache **row)
{
  Item *sel_item;
  List_iterator_fast<Item> li(item_list);
  cmp_type= res_type= STRING_RESULT;
  res_field_type= MYSQL_TYPE_VAR_STRING;
  for (uint i= 0; (sel_item= li++); i++)
  {
    item->max_length= sel_item->max_length;
    res_type= sel_item->result_type();
    cmp_type= sel_item->cmp_type();
    res_field_type= sel_item->field_type();
    item->decimals= sel_item->decimals;
    item->unsigned_flag= sel_item->unsigned_flag;
    maybe_null= sel_item->maybe_null;
    if (!(row[i]= Item_cache::get_cache(thd, sel_item, sel_item->cmp_type())))
      return TRUE;
    row[i]->setup(thd, sel_item);
 //psergey-backport-timours:   row[i]->store(sel_item);
  }
  if (item_list.elements > 1)
    cmp_type= res_type= ROW_RESULT;
  return FALSE;
}