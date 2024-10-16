Ordered_key::cmp_keys_by_row_data_and_rownum(Ordered_key *key,
                                             rownum_t* a, rownum_t* b)
{
  /* The result of comparing the two keys according to their row data. */
  int cmp_row_res= key->cmp_keys_by_row_data(*a, *b);
  if (cmp_row_res)
    return cmp_row_res;
  return (*a < *b) ? -1 : (*a > *b) ? 1 : 0;
}