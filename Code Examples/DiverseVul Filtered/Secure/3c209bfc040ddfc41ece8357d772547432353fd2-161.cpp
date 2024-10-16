int subselect_single_select_engine::exec()
{
  DBUG_ENTER("subselect_single_select_engine::exec");

  char const *save_where= thd->where;
  SELECT_LEX *save_select= thd->lex->current_select;
  thd->lex->current_select= select_lex;

  if (join->optimization_state == JOIN::NOT_OPTIMIZED)
  {
    SELECT_LEX_UNIT *unit= select_lex->master_unit();

    unit->set_limit(unit->global_parameters());
    if (join->optimize())
    {
      thd->where= save_where;
      executed= 1;
      thd->lex->current_select= save_select;
      DBUG_RETURN(join->error ? join->error : 1);
    }
    if (!select_lex->uncacheable && thd->lex->describe && 
        !(join->select_options & SELECT_DESCRIBE))
    {
      item->update_used_tables();
      if (item->const_item())
      {
        /*
          It's necessary to keep original JOIN table because
          create_sort_index() function may overwrite original
          JOIN_TAB::type and wrong optimization method can be
          selected on re-execution.
        */
        select_lex->uncacheable|= UNCACHEABLE_EXPLAIN;
        select_lex->master_unit()->uncacheable|= UNCACHEABLE_EXPLAIN;
      }
    }
    if (item->engine_changed(this))
    {
      thd->lex->current_select= save_select;
      DBUG_RETURN(1);
    }
  }
  if (select_lex->uncacheable &&
      select_lex->uncacheable != UNCACHEABLE_EXPLAIN
      && executed)
  {
    if (join->reinit())
    {
      thd->where= save_where;
      thd->lex->current_select= save_select;
      DBUG_RETURN(1);
    }
    item->reset();
    item->assigned((executed= 0));
  }
  if (!executed)
  {
    item->reset_value_registration();
    JOIN_TAB *changed_tabs[MAX_TABLES];
    JOIN_TAB **last_changed_tab= changed_tabs;
    if (item->have_guarded_conds())
    {
      /*
        For at least one of the pushed predicates the following is true:
        We should not apply optimizations based on the condition that was
        pushed down into the subquery. Those optimizations are ref[_or_null]
        accesses. Change them to be full table scans.
      */
      JOIN_TAB *tab;
      for (tab= first_linear_tab(join, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
           tab; tab= next_linear_tab(join, tab, WITH_BUSH_ROOTS))
      {
        if (tab && tab->keyuse)
        {
          for (uint i= 0; i < tab->ref.key_parts; i++)
          {
            bool *cond_guard= tab->ref.cond_guards[i];
            if (cond_guard && !*cond_guard)
            {
              /* Change the access method to full table scan */
              tab->save_read_first_record= tab->read_first_record;
              tab->save_read_record= tab->read_record.read_record;
              tab->read_record.read_record= rr_sequential;
              tab->read_first_record= read_first_record_seq;
              tab->read_record.record= tab->table->record[0];
              tab->read_record.thd= join->thd;
              tab->read_record.ref_length= tab->table->file->ref_length;
              tab->read_record.unlock_row= rr_unlock_row;
              *(last_changed_tab++)= tab;
              break;
            }
          }
        }
      }
    }
    
    join->exec();

    /* Enable the optimizations back */
    for (JOIN_TAB **ptab= changed_tabs; ptab != last_changed_tab; ptab++)
    {
      JOIN_TAB *tab= *ptab;
      tab->read_record.record= 0;
      tab->read_record.ref_length= 0;
      tab->read_first_record= tab->save_read_first_record; 
      tab->read_record.read_record= tab->save_read_record;
    }
    executed= 1;
    if (!(uncacheable() & ~UNCACHEABLE_EXPLAIN) &&
        !item->with_recursive_reference)
      item->make_const();
    thd->where= save_where;
    thd->lex->current_select= save_select;
    DBUG_RETURN(join->error || thd->is_fatal_error || thd->is_error());
  }
  thd->where= save_where;
  thd->lex->current_select= save_select;
  DBUG_RETURN(0);
}