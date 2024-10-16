check_result_t handler_rowid_filter_check(void *h_arg)
{
  handler *h= (handler*) h_arg;
  TABLE *tab= h->get_table();

  /*
    Check for out-of-range and killed conditions only if we haven't done it
    already in the pushed index condition check
  */
  if (!h->pushed_idx_cond)
  {
    THD *thd= h->table->in_use;
    DEBUG_SYNC(thd, "handler_rowid_filter_check");
    enum thd_kill_levels abort_at= h->has_transactions() ?
      THD_ABORT_SOFTLY : THD_ABORT_ASAP;
    if (thd_kill_level(thd) > abort_at)
      return CHECK_ABORTED_BY_USER;

    if (h->end_range && h->compare_key2(h->end_range) > 0)
      return CHECK_OUT_OF_RANGE;
  }

  h->position(tab->record[0]);
  return h->pushed_rowid_filter->check((char*)h->ref)? CHECK_POS: CHECK_NEG;
}