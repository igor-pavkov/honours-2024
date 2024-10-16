static void conn_reset_postponed_data(struct connectdata *conn, int num)
{
  struct postponed_data * const psnd = &(conn->postponed[num]);
  if(psnd->buffer) {
    DEBUGASSERT(psnd->allocated_size > 0);
    DEBUGASSERT(psnd->recv_size <= psnd->allocated_size);
    DEBUGASSERT(psnd->recv_size ?
                (psnd->recv_processed < psnd->recv_size) :
                (psnd->recv_processed == 0));
    DEBUGASSERT(psnd->bindsock != CURL_SOCKET_BAD);
    free(psnd->buffer);
    psnd->buffer = NULL;
    psnd->allocated_size = 0;
    psnd->recv_size = 0;
    psnd->recv_processed = 0;
#ifdef DEBUGBUILD
    psnd->bindsock = CURL_SOCKET_BAD; /* used only for DEBUGASSERT */
#endif /* DEBUGBUILD */
  }
  else {
    DEBUGASSERT(psnd->allocated_size == 0);
    DEBUGASSERT(psnd->recv_size == 0);
    DEBUGASSERT(psnd->recv_processed == 0);
    DEBUGASSERT(psnd->bindsock == CURL_SOCKET_BAD);
  }
}