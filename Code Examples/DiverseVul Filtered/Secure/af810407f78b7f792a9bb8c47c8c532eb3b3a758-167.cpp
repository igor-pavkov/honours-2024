extern "C" check_result_t handler_index_cond_check(void* h_arg)
{
  handler *h= (handler*)h_arg;
  THD *thd= h->table->in_use;
  check_result_t res;

  DEBUG_SYNC(thd, "handler_index_cond_check");
  enum thd_kill_levels abort_at= h->has_transactions() ?
    THD_ABORT_SOFTLY : THD_ABORT_ASAP;
  if (thd_kill_level(thd) > abort_at)
    return CHECK_ABORTED_BY_USER;

  if (h->end_range && h->compare_key2(h->end_range) > 0)
    return CHECK_OUT_OF_RANGE;
  h->increment_statistics(&SSV::ha_icp_attempts);
  if ((res= h->pushed_idx_cond->val_int()? CHECK_POS : CHECK_NEG) ==
      CHECK_POS)
    h->increment_statistics(&SSV::ha_icp_match);
  return res;
}