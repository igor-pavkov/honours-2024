bool Item_subselect::fix_fields(THD *thd_param, Item **ref)
{
  char const *save_where= thd_param->where;
  uint8 uncacheable;
  bool res;

  thd= thd_param;

  DBUG_ASSERT(unit->thd == thd);

  status_var_increment(thd_param->status_var.feature_subquery);

  DBUG_ASSERT(fixed == 0);
  engine->set_thd((thd= thd_param));
  if (!done_first_fix_fields)
  {
    done_first_fix_fields= TRUE;
    inside_first_fix_fields= TRUE;
    upper_refs.empty();
    /*
      psergey-todo: remove _first_fix_fields calls, we need changes on every
      execution
    */
  }

  eliminated= FALSE;
  parent_select= thd_param->lex->current_select;

  if (check_stack_overrun(thd, STACK_MIN_SIZE, (uchar*)&res))
    return TRUE;
  
  
  if (!(res= engine->prepare(thd)))
  {
    // all transformation is done (used by prepared statements)
    changed= 1;
    inside_first_fix_fields= FALSE;

    /*
      Substitute the current item with an Item_in_optimizer that was
      created by Item_in_subselect::select_in_like_transformer and
      call fix_fields for the substituted item which in turn calls
      engine->prepare for the subquery predicate.
    */
    if (substitution)
    {
      /*
        If the top item of the WHERE/HAVING condition changed,
        set correct WHERE/HAVING for PS.
      */
      if (unit->outer_select()->where == (*ref))
        unit->outer_select()->where= substitution;
      else if (unit->outer_select()->having == (*ref))
        unit->outer_select()->having= substitution;

      (*ref)= substitution;
      substitution->name= name;
      substitution->name_length= name_length;
      if (have_to_be_excluded)
	engine->exclude();
      substitution= 0;
      thd->where= "checking transformed subquery";
      if (!(*ref)->fixed)
	res= (*ref)->fix_fields(thd, ref);
      goto end;

    }
    // Is it one field subselect?
    if (engine->cols() > max_columns)
    {
      my_error(ER_OPERAND_COLUMNS, MYF(0), 1);
      res= TRUE;
      goto end;
    }
    if (fix_length_and_dec())
    {
      res= TRUE;
      goto end;
    }
  }
  else
    goto end;
  
  if ((uncacheable= engine->uncacheable() & ~UNCACHEABLE_EXPLAIN) ||
      with_recursive_reference)
  {
    const_item_cache= 0;
    if (uncacheable & UNCACHEABLE_RAND)
      used_tables_cache|= RAND_TABLE_BIT;
  }
  fixed= 1;

end:
  done_first_fix_fields= FALSE;
  inside_first_fix_fields= FALSE;
  thd->where= save_where;
  return res;
}