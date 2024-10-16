bool subselect_union_engine::change_result(Item_subselect *si,
                                           select_result_interceptor *res,
                                           bool temp)
{
  item= si;
  int rc= unit->change_result(res, result);
  if (temp)
    thd->change_item_tree((Item**) &result, (Item*)res);
  else
    result= res;
  return rc;
}