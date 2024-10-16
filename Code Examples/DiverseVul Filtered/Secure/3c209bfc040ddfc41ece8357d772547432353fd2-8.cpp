Item_singlerow_subselect::select_transformer(JOIN *join)
{
  DBUG_ENTER("Item_singlerow_subselect::select_transformer");
  if (changed)
    DBUG_RETURN(false);
  DBUG_ASSERT(join->thd == thd);

  SELECT_LEX *select_lex= join->select_lex;
  Query_arena *arena= thd->stmt_arena;

  if (!select_lex->master_unit()->is_union() &&
      !select_lex->table_list.elements &&
      select_lex->item_list.elements == 1 &&
      !select_lex->item_list.head()->with_sum_func &&
      /*
	We can't change name of Item_field or Item_ref, because it will
	prevent its correct resolving, but we should save name of
	removed item => we do not make optimization if top item of
	list is field or reference.
	TODO: solve above problem
      */
      !(select_lex->item_list.head()->type() == FIELD_ITEM ||
	select_lex->item_list.head()->type() == REF_ITEM) &&
      !join->conds && !join->having &&
      /*
        switch off this optimization for prepare statement,
        because we do not rollback this changes
        TODO: make rollback for it, or special name resolving mode in 5.0.
      */
      !arena->is_stmt_prepare_or_first_sp_execute()
      )
  {
    have_to_be_excluded= 1;
    if (thd->lex->describe)
    {
      char warn_buff[MYSQL_ERRMSG_SIZE];
      sprintf(warn_buff, ER_THD(thd, ER_SELECT_REDUCED),
              select_lex->select_number);
      push_warning(thd, Sql_condition::WARN_LEVEL_NOTE,
		   ER_SELECT_REDUCED, warn_buff);
    }
    substitution= select_lex->item_list.head();
    /*
      as far as we moved content to upper level we have to fix dependences & Co
    */
    substitution->fix_after_pullout(select_lex->outer_select(),
                                    &substitution, TRUE);
  }
  DBUG_RETURN(false);
}