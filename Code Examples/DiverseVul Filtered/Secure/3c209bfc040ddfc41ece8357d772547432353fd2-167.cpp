bool subselect_hash_sj_engine::init(List<Item> *tmp_columns, uint subquery_id)
{
  THD *thd= get_thd();
  select_union *result_sink;
  /* Options to create_tmp_table. */
  ulonglong tmp_create_options= thd->variables.option_bits | TMP_TABLE_ALL_COLUMNS;
                             /* | TMP_TABLE_FORCE_MYISAM; TIMOUR: force MYISAM */

  DBUG_ENTER("subselect_hash_sj_engine::init");

  if (my_bitmap_init_memroot(&non_null_key_parts, tmp_columns->elements,
                            thd->mem_root) ||
      my_bitmap_init_memroot(&partial_match_key_parts, tmp_columns->elements,
                            thd->mem_root))
    DBUG_RETURN(TRUE);

  /*
    Create and initialize a select result interceptor that stores the
    result stream in a temporary table. The temporary table itself is
    managed (created/filled/etc) internally by the interceptor.
  */
/*
  TIMOUR:
  Select a more efficient result sink when we know there is no need to collect
  data statistics.

  if (strategy == COMPLETE_MATCH)
  {
    if (!(result= new select_union))
      DBUG_RETURN(TRUE);
  }
  else if (strategy == PARTIAL_MATCH)
  {
  if (!(result= new select_materialize_with_stats))
    DBUG_RETURN(TRUE);
  }
*/
  if (!(result_sink= new (thd->mem_root) select_materialize_with_stats(thd)))
    DBUG_RETURN(TRUE);
    
  char buf[32];
  uint len= my_snprintf(buf, sizeof(buf), "<subquery%d>", subquery_id);
  char *name;
  if (!(name= (char*)thd->alloc(len + 1)))
    DBUG_RETURN(TRUE);
  memcpy(name, buf, len+1);

  result_sink->get_tmp_table_param()->materialized_subquery= true;
  if (item->substype() == Item_subselect::IN_SUBS && 
      ((Item_in_subselect*)item)->is_jtbm_merged)
  {
    result_sink->get_tmp_table_param()->force_not_null_cols= true;
  }
  if (result_sink->create_result_table(thd, tmp_columns, TRUE,
                                       tmp_create_options,
				       name, TRUE, TRUE))
    DBUG_RETURN(TRUE);

  tmp_table= result_sink->table;
  result= result_sink;

  /*
    If the subquery has blobs, or the total key length is bigger than
    some length, or the total number of key parts is more than the
    allowed maximum (currently MAX_REF_PARTS == 32), then the created
    index cannot be used for lookups and we can't use hash semi
    join. If this is the case, delete the temporary table since it
    will not be used, and tell the caller we failed to initialize the
    engine.
  */
  if (tmp_table->s->keys == 0)
  {
    //fprintf(stderr, "Q: %s\n", current_thd->query());
    DBUG_ASSERT(0);
    DBUG_ASSERT(
      tmp_table->s->uniques ||
      tmp_table->key_info->key_length >= tmp_table->file->max_key_length() ||
      tmp_table->key_info->user_defined_key_parts >
      tmp_table->file->max_key_parts());
    free_tmp_table(thd, tmp_table);
    tmp_table= NULL;
    delete result;
    result= NULL;
    DBUG_RETURN(TRUE);
  }

  /*
    Make sure there is only one index on the temp table, and it doesn't have
    the extra key part created when s->uniques > 0.
  */
  DBUG_ASSERT(tmp_table->s->keys == 1 &&
              ((Item_in_subselect *) item)->left_expr->cols() ==
              tmp_table->key_info->user_defined_key_parts);

  if (make_semi_join_conds() ||
      /* A unique_engine is used both for complete and partial matching. */
      !(lookup_engine= make_unique_engine()))
    DBUG_RETURN(TRUE);

  /*
    Repeat name resolution for 'cond' since cond is not part of any
    clause of the query, and it is not 'fixed' during JOIN::prepare.
  */
  if (semi_join_conds && !semi_join_conds->fixed &&
      semi_join_conds->fix_fields(thd, (Item**)&semi_join_conds))
    DBUG_RETURN(TRUE);
  /* Let our engine reuse this query plan for materialization. */
  materialize_join= materialize_engine->join;
  materialize_join->change_result(result, NULL);

  DBUG_RETURN(FALSE);
}