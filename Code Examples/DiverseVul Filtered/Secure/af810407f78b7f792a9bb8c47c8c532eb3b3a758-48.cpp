void handler::update_global_index_stats()
{
  DBUG_ASSERT(table->s);

  if (!table->in_use->userstat_running)
  {
    /* Reset all index read values */
    bzero(index_rows_read, sizeof(index_rows_read[0]) * table->s->keys);
    return;
  }

  for (uint index = 0; index < table->s->keys; index++)
  {
    if (index_rows_read[index])
    {
      INDEX_STATS* index_stats;
      size_t key_length;
      KEY *key_info = &table->key_info[index];  // Rows were read using this

      DBUG_ASSERT(key_info->cache_name);
      if (!key_info->cache_name)
        continue;
      key_length= table->s->table_cache_key.length + key_info->name.length + 1;
      mysql_mutex_lock(&LOCK_global_index_stats);
      // Gets the global index stats, creating one if necessary.
      if (!(index_stats= (INDEX_STATS*) my_hash_search(&global_index_stats,
                                                    key_info->cache_name,
                                                    key_length)))
      {
        if (!(index_stats = ((INDEX_STATS*)
                             my_malloc(sizeof(INDEX_STATS),
                                       MYF(MY_WME | MY_ZEROFILL)))))
          goto end;                             // Error is already given

        memcpy(index_stats->index, key_info->cache_name, key_length);
        index_stats->index_name_length= key_length;
        if (my_hash_insert(&global_index_stats, (uchar*) index_stats))
        {
          my_free(index_stats);
          goto end;
        }
      }
      /* Updates the global index stats. */
      index_stats->rows_read+= index_rows_read[index];
      index_rows_read[index]= 0;
end:
      mysql_mutex_unlock(&LOCK_global_index_stats);
    }
  }
}