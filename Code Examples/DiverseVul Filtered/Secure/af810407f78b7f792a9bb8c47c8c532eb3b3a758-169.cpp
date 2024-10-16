int del_global_index_stat(THD *thd, TABLE* table, KEY* key_info)
{
  INDEX_STATS *index_stats;
  size_t key_length= table->s->table_cache_key.length + key_info->name.length + 1;
  int res = 0;
  DBUG_ENTER("del_global_index_stat");
  mysql_mutex_lock(&LOCK_global_index_stats);

  if((index_stats= (INDEX_STATS*) my_hash_search(&global_index_stats,
                                                key_info->cache_name,
                                                key_length)))
    res= my_hash_delete(&global_index_stats, (uchar*)index_stats);

  mysql_mutex_unlock(&LOCK_global_index_stats);
  DBUG_RETURN(res);
}