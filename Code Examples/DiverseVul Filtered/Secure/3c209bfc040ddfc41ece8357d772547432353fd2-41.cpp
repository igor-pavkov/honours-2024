subselect_rowid_merge_engine::cmp_keys_by_cur_rownum(void *arg,
                                                     uchar *k1, uchar *k2)
{
  rownum_t r1= ((Ordered_key*) k1)->current();
  rownum_t r2= ((Ordered_key*) k2)->current();

  return (r1 < r2) ? -1 : (r1 > r2) ? 1 : 0;
}