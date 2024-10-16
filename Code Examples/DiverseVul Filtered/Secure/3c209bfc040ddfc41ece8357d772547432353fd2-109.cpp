Ordered_key::cmp_keys_by_row_data(ha_rows a, ha_rows b)
{
  uchar *rowid_a, *rowid_b;
  int __attribute__((unused)) error;
  int cmp_res;
  /* The length in bytes of the rowids (positions) of tmp_table. */
  uint rowid_length= tbl->file->ref_length;

  if (a == b)
    return 0;
  /* Get the corresponding rowids. */
  rowid_a= row_num_to_rowid + a * rowid_length;
  rowid_b= row_num_to_rowid + b * rowid_length;
  /* Fetch the rows for comparison. */
  if ((error= tbl->file->ha_rnd_pos(tbl->record[0], rowid_a)))
  {
    /* purecov: begin inspected */
    tbl->file->print_error(error, MYF(ME_FATALERROR));  // Sets fatal_error
    return 0;
    /* purecov: end */
  }
  if ((error= tbl->file->ha_rnd_pos(tbl->record[1], rowid_b)))
  {
    /* purecov: begin inspected */
    tbl->file->print_error(error, MYF(ME_FATALERROR));  // Sets fatal_error
    return 0;
    /* purecov: end */
  }    
  /*
    Compare the two rows by the corresponding values of the indexed
    columns.
  */
  for (uint i= 0; i < key_column_count; i++)
  {
    Field *cur_field= key_columns[i]->field;
    if ((cmp_res= cur_field->cmp_offset(tbl->s->rec_buff_length)))
      return (cmp_res > 0 ? 1 : -1);
  }
  return 0;
}