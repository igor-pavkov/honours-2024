int ha_change_key_cache_param(KEY_CACHE *key_cache)
{
  DBUG_ENTER("ha_change_key_cache_param");

  if (key_cache->key_cache_inited)
  {
    mysql_mutex_lock(&LOCK_global_system_variables);
    uint division_limit= (uint)key_cache->param_division_limit;
    uint age_threshold=  (uint)key_cache->param_age_threshold;
    mysql_mutex_unlock(&LOCK_global_system_variables);
    change_key_cache_param(key_cache, division_limit, age_threshold);
  }
  DBUG_RETURN(0);
}