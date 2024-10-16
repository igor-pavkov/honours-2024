int del_global_table_stat(THD *thd, const LEX_CSTRING *db, const LEX_CSTRING *table)
{
  TABLE_STATS *table_stats;
  int res = 0;
  uchar *cache_key;
  size_t cache_key_length;
  DBUG_ENTER("del_global_table_stat");

  cache_key_length= db->length + 1 + table->length + 1;

  if(!(cache_key= (uchar *)my_malloc(cache_key_length,
                                     MYF(MY_WME | MY_ZEROFILL))))
  {
    /* Out of memory error already given */
    res = 1;
    goto end;
  }

  memcpy(cache_key, db->str, db->length);
  memcpy(cache_key + db->length + 1, table->str, table->length);

  res= del_global_index_stats_for_table(thd, cache_key, cache_key_length);

  mysql_mutex_lock(&LOCK_global_table_stats);

  if((table_stats= (TABLE_STATS*) my_hash_search(&global_table_stats,
                                                cache_key,
                                                cache_key_length)))
    res= my_hash_delete(&global_table_stats, (uchar*)table_stats);

  my_free(cache_key);
  mysql_mutex_unlock(&LOCK_global_table_stats);

end:
  DBUG_RETURN(res);
}