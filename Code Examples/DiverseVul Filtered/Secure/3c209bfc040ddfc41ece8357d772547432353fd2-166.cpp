void Item_allany_subselect::cleanup()
{
  /*
    The MAX/MIN transformation through injection is reverted through the
    change_item_tree() mechanism. Revert the select_lex object of the
    query to its initial state.
  */
  for (SELECT_LEX *sl= unit->first_select();
       sl; sl= sl->next_select())
    if (test_set_strategy(SUBS_MAXMIN_INJECTED))
      sl->with_sum_func= false;
  Item_in_subselect::cleanup();
}