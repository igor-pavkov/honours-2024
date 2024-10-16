static void prune_dead_connections(struct Curl_easy *data)
{
  struct curltime now = Curl_now();
  time_t elapsed = Curl_timediff(now, data->state.conn_cache->last_cleanup);

  if(elapsed >= 1000L) {
    struct prunedead prune;
    prune.data = data;
    prune.extracted = NULL;
    while(Curl_conncache_foreach(data, data->state.conn_cache, &prune,
                                 call_extract_if_dead)) {
      /* disconnect it */
      (void)Curl_disconnect(data, prune.extracted, /* dead_connection */TRUE);
    }
    data->state.conn_cache->last_cleanup = now;
  }
}