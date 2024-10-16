bool Item_subselect::set_fake_select_as_master_processor(void *arg)
{
  SELECT_LEX *fake_select= (SELECT_LEX*) arg;
  /*
    Move the st_select_lex_unit of a subquery from a global ORDER BY clause to
    become a direct child of the fake_select of a UNION. In this way the
    ORDER BY that is applied to the temporary table that contains the result of
    the whole UNION, and all columns in the subquery are resolved against this
    table. The transformation is applied only for immediate child subqueries of
    a UNION query.
  */
  if (unit->outer_select()->master_unit()->fake_select_lex == fake_select)
  {
    /*
      Set the master of the subquery to be the fake select (i.e. the whole
      UNION), instead of the last query in the UNION.
    */
    fake_select->add_slave(unit);
    DBUG_ASSERT(unit->outer_select() == fake_select);
    /* Adjust the name resolution context hierarchy accordingly. */
    for (SELECT_LEX *sl= unit->first_select(); sl; sl= sl->next_select())
      sl->context.outer_context= &(fake_select->context);
    /*
      Undo Item_subselect::eliminate_subselect_processor because at that phase
      we don't know yet that the ORDER clause will be moved to the fake select.
    */
    unit->item= this;
    eliminated= FALSE;
  }
  return FALSE;
}