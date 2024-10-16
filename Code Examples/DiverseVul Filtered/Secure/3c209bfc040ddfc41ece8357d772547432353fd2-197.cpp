int subselect_single_select_engine::prepare(THD *thd)
{
  if (prepared)
    return 0;
  set_thd(thd);
  if (select_lex->join)
  {
    select_lex->cleanup();
  }
  join= new JOIN(thd, select_lex->item_list,
		 select_lex->options | SELECT_NO_UNLOCK, result);
  if (!join || !result)
    return 1; /* Fatal error is set already. */
  prepared= 1;
  SELECT_LEX *save_select= thd->lex->current_select;
  thd->lex->current_select= select_lex;
  if (join->prepare(select_lex->table_list.first,
		    select_lex->with_wild,
		    select_lex->where,
		    select_lex->order_list.elements +
		    select_lex->group_list.elements,
		    select_lex->order_list.first,
                    false,
		    select_lex->group_list.first,
		    select_lex->having,
		    NULL, select_lex,
		    select_lex->master_unit()))
    return 1;
  thd->lex->current_select= save_select;
  return 0;
}