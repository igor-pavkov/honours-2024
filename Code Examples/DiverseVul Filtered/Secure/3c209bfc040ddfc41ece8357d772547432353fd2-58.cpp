subselect_single_select_engine::change_result(Item_subselect *si,
                                              select_result_interceptor *res,
                                              bool temp)
{
  DBUG_ENTER("subselect_single_select_engine::change_result");
  item= si;
  if (temp)
  {
    /*
      Here we reuse change_item_tree to roll back assignment.  It has
      nothing special about Item* pointer so it is safe conversion. We do
      not change the interface to be compatible with MySQL.
    */
    thd->change_item_tree((Item**) &result, (Item*)res);
  }
  else
    result= res;

  /*
    We can't use 'result' below as gcc 4.2.4's alias optimization
    assumes that result was not changed by thd->change_item_tree().
    I tried to find a solution to make gcc happy, but could not find anything
    that would not require a lot of extra code that would be harder to manage
    than the current code.
  */
  DBUG_RETURN(select_lex->join->change_result(res, NULL));
}