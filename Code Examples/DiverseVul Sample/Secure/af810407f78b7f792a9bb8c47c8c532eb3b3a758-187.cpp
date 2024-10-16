int ha_init_key_cache(const char *name, KEY_CACHE *key_cache, void *unused
                      __attribute__((unused)))
{
  DBUG_ENTER("ha_init_key_cache");

  if (!key_cache->key_cache_inited)
  {
    mysql_mutex_lock(&LOCK_global_system_variables);
    size_t tmp_buff_size= (size_t) key_cache->param_buff_size;
    uint tmp_block_size= (uint) key_cache->param_block_size;
    uint division_limit= (uint)key_cache->param_division_limit;
    uint age_threshold=  (uint)key_cache->param_age_threshold;
    uint partitions=     (uint)key_cache->param_partitions;
    uint changed_blocks_hash_size=  (uint)key_cache->changed_blocks_hash_size;
    mysql_mutex_unlock(&LOCK_global_system_variables);
    DBUG_RETURN(!init_key_cache(key_cache,
				tmp_block_size,
				tmp_buff_size,
				division_limit, age_threshold,
                                changed_blocks_hash_size,
                                partitions));
  }
  DBUG_RETURN(0);
}