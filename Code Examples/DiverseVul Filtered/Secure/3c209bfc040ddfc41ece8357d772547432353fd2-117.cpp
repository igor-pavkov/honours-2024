void subselect_uniquesubquery_engine::cleanup()
{
  DBUG_ENTER("subselect_uniquesubquery_engine::cleanup");
  /* 
    Note for mergers: we don't have to, and actually must not de-initialize
    tab->table->file here.
    - We don't have to, because free_tmp_table() will call ha_index_or_rnd_end
    - We must not do it, because tab->table may be a derived table which 
      has been already dropped by close_thread_tables(), while we here are
      called from cleanup_items()
  */
  DBUG_VOID_RETURN;
}