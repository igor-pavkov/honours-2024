Item_subselect::Item_subselect(THD *thd_arg):
  Item_result_field(thd_arg), Used_tables_and_const_cache(),
  value_assigned(0), own_engine(0), thd(0), old_engine(0),
  have_to_be_excluded(0),
  inside_first_fix_fields(0), done_first_fix_fields(FALSE), 
  expr_cache(0), forced_const(FALSE), substitution(0), engine(0), eliminated(FALSE),
  changed(0), is_correlated(FALSE), with_recursive_reference(0)
{
  DBUG_ENTER("Item_subselect::Item_subselect");
  DBUG_PRINT("enter", ("this: %p", this));
  sortbuffer.str= 0;

#ifndef DBUG_OFF
  exec_counter= 0;
#endif
  with_subselect= 1;
  reset();
  /*
    Item value is NULL if select_result_interceptor didn't change this value
    (i.e. some rows will be found returned)
  */
  null_value= TRUE;
  DBUG_VOID_RETURN;
}