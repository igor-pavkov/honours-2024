static my_xid wsrep_order_and_check_continuity(XID *list, int len)
{
#ifdef WITH_WSREP
  wsrep_sort_xid_array(list, len);
  wsrep::gtid cur_position= wsrep_get_SE_checkpoint();
  long long cur_seqno= cur_position.seqno().get();
  for (int i= 0; i < len; ++i)
  {
    if (!wsrep_is_wsrep_xid(list + i) ||
        wsrep_xid_seqno(list + i) != cur_seqno + 1)
    {
      WSREP_WARN("Discovered discontinuity in recovered wsrep "
                 "transaction XIDs. Truncating the recovery list to "
                 "%d entries", i);
      break;
    }
    ++cur_seqno;
  }
  WSREP_INFO("Last wsrep seqno to be recovered %lld", cur_seqno);
  return (cur_seqno < 0 ? 0 : cur_seqno);
#else
  return 0;
#endif /* WITH_WSREP */
}