void handler::use_hidden_primary_key()
{
  /* fallback to use all columns in the table to identify row */
  table->column_bitmaps_set(&table->s->all_set, table->write_set);
}