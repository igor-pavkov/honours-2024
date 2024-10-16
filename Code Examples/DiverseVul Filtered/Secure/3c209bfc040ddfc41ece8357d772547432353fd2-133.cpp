Item_maxmin_subselect::Item_maxmin_subselect(THD *thd,
                                             Item_subselect *parent,
					     st_select_lex *select_lex,
					     bool max_arg):
  Item_singlerow_subselect(thd), was_values(TRUE)
{
  DBUG_ENTER("Item_maxmin_subselect::Item_maxmin_subselect");
  max= max_arg;
  init(select_lex,
       new (thd->mem_root) select_max_min_finder_subselect(thd,
             this, max_arg, parent->substype() == Item_subselect::ALL_SUBS));
  max_columns= 1;
  maybe_null= 1;
  max_columns= 1;

  /*
    Following information was collected during performing fix_fields()
    of Items belonged to subquery, which will be not repeated
  */
  used_tables_cache= parent->get_used_tables_cache();
  const_item_cache= parent->const_item();

  DBUG_VOID_RETURN;
}