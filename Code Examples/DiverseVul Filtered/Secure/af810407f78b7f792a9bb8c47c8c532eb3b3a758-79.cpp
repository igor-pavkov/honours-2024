int del_global_index_stats_for_table(THD *thd, uchar* cache_key, size_t cache_key_length)
{
  int res = 0;
  DBUG_ENTER("del_global_index_stats_for_table");

  mysql_mutex_lock(&LOCK_global_index_stats);

  for (uint i= 0; i < global_index_stats.records;)
  {
    INDEX_STATS *index_stats =
      (INDEX_STATS*) my_hash_element(&global_index_stats, i);

    /* We search correct db\0table_name\0 string */
    if (index_stats &&
	index_stats->index_name_length >= cache_key_length &&
	!memcmp(index_stats->index, cache_key, cache_key_length))
    {
      res= my_hash_delete(&global_index_stats, (uchar*)index_stats);
      /*
          In our HASH implementation on deletion one elements
          is moved into a place where a deleted element was,
          and the last element is moved into the empty space.
          Thus we need to re-examine the current element, but
          we don't have to restart the search from the beginning.
      */
    }
    else
      i++;
  }

  mysql_mutex_unlock(&LOCK_global_index_stats);
  DBUG_RETURN(res);
}