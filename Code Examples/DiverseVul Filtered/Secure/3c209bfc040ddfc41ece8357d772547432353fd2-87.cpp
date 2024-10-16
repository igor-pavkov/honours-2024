int Ordered_key::cmp_key_with_search_key(rownum_t row_num)
{
  /* The length in bytes of the rowids (positions) of tmp_table. */
  uint rowid_length= tbl->file->ref_length;
  uchar *cur_rowid= row_num_to_rowid + row_num * rowid_length;
  int __attribute__((unused)) error;
  int cmp_res;

  if ((error= tbl->file->ha_rnd_pos(tbl->record[0], cur_rowid)))
  {
    /* purecov: begin inspected */
    tbl->file->print_error(error, MYF(ME_FATALERROR));  // Sets fatal_error
    return 0;
    /* purecov: end */
  }

  for (uint i= 0; i < key_column_count; i++)
  {
    cmp_res= compare_pred[i]->get_comparator()->compare();
    /* Unlike Arg_comparator::compare_row() here there should be no NULLs. */
    DBUG_ASSERT(!compare_pred[i]->null_value);
    if (cmp_res)
      return (cmp_res > 0 ? 1 : -1);
  }
  return 0;
}