void Item_subselect::init(st_select_lex *select_lex,
			  select_result_interceptor *result)
{
  /*
    Please see Item_singlerow_subselect::invalidate_and_restore_select_lex(),
    which depends on alterations to the parse tree implemented here.
  */

  DBUG_ENTER("Item_subselect::init");
  DBUG_PRINT("enter", ("select_lex: %p  this: %p",
                       select_lex, this));
  unit= select_lex->master_unit();

  if (unit->item)
  {
    engine= unit->item->engine;
    parsing_place= unit->item->parsing_place;
    if (unit->item->substype() == EXISTS_SUBS &&
        ((Item_exists_subselect *)unit->item)->exists_transformed)
    {
      /* it is permanent transformation of EXISTS to IN */
      unit->item= this;
      engine->change_result(this, result, FALSE);
    }
    else
    {
      /*
        Item can be changed in JOIN::prepare while engine in JOIN::optimize
        => we do not copy old_engine here
      */
      unit->thd->change_item_tree((Item**)&unit->item, this);
      engine->change_result(this, result, TRUE);
    }
  }
  else
  {
    SELECT_LEX *outer_select= unit->outer_select();
    /*
      do not take into account expression inside aggregate functions because
      they can access original table fields
    */
    parsing_place= (outer_select->in_sum_expr ? NO_MATTER
                                              : outer_select->parsing_place);
    if (unit->is_union())
      engine= new subselect_union_engine(unit, result, this);
    else
      engine= new subselect_single_select_engine(select_lex, result, this);
  }
  {
    SELECT_LEX *upper= unit->outer_select();
    if (upper->parsing_place == IN_HAVING)
      upper->subquery_in_having= 1;
    /* The subquery is an expression cache candidate */
    upper->expr_cache_may_be_used[upper->parsing_place]= TRUE;
  }
  DBUG_PRINT("info", ("engine: %p", engine));
  DBUG_VOID_RETURN;
}