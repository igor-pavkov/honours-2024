SSLNetVConnection::free(EThread *t)
{
  ink_release_assert(t == this_ethread());

  // cancel OOB
  cancel_OOB();
  // close socket fd
  if (con.fd != NO_FD) {
    NET_SUM_GLOBAL_DYN_STAT(net_connections_currently_open_stat, -1);
  }
  con.close();

  clear();
  SET_CONTINUATION_HANDLER(this, (SSLNetVConnHandler)&SSLNetVConnection::startEvent);
  ink_assert(con.fd == NO_FD);
  ink_assert(t == this_ethread());

  if (from_accept_thread) {
    sslNetVCAllocator.free(this);
  } else {
    ink_assert(con.fd == NO_FD);
    THREAD_FREE(this, sslNetVCAllocator, t);
  }
}