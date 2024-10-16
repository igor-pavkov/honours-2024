subselect_rowid_merge_engine::cmp_keys_by_null_selectivity(Ordered_key **k1,
                                                           Ordered_key **k2)
{
  double k1_sel= (*k1)->null_selectivity();
  double k2_sel= (*k2)->null_selectivity();
  if (k1_sel < k2_sel)
    return 1;
  if (k1_sel > k2_sel)
    return -1;
  return 0;
}