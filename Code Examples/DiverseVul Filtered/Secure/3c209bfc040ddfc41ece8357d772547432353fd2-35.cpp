Item_singlerow_subselect::invalidate_and_restore_select_lex()
{
  DBUG_ENTER("Item_singlerow_subselect::invalidate_and_restore_select_lex");
  st_select_lex *result= get_select_lex();

  DBUG_ASSERT(result);

  /*
    This code restore the parse tree in it's state before the execution of
    Item_singlerow_subselect::Item_singlerow_subselect(),
    and in particular decouples this object from the SELECT_LEX,
    so that the SELECT_LEX can be used with a different flavor
    or Item_subselect instead, as part of query rewriting.
  */
  unit->item= NULL;

  DBUG_RETURN(result);
}