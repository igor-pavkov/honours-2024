int handler::calculate_checksum()
{
  int error;
  THD *thd=ha_thd();
  DBUG_ASSERT(table->s->last_null_bit_pos < 8);
  uchar null_mask= table->s->last_null_bit_pos
                   ? 256 -  (1 << table->s->last_null_bit_pos) : 0;

  table->use_all_stored_columns();
  stats.checksum= 0;

  if ((error= ha_rnd_init(1)))
    return error;

  for (;;)
  {
    if (thd->killed)
      return HA_ERR_ABORTED_BY_USER;

    ha_checksum row_crc= 0;
    error= ha_rnd_next(table->record[0]);
    if (error)
      break;

    if (table->s->null_bytes)
    {
      /* fix undefined null bits */
      table->record[0][table->s->null_bytes-1] |= null_mask;
      if (!(table->s->db_create_options & HA_OPTION_PACK_RECORD))
        table->record[0][0] |= 1;

      row_crc= my_checksum(row_crc, table->record[0], table->s->null_bytes);
    }

    uchar *checksum_start= NULL;
    size_t checksum_length= 0;
    for (uint i= 0; i < table->s->fields; i++ )
    {
      Field *f= table->field[i];

      if (! thd->variables.old_mode && f->is_real_null(0))
      {
        flush_checksum(&row_crc, &checksum_start, &checksum_length);
        continue;
      }
     /*
       BLOB and VARCHAR have pointers in their field, we must convert
       to string; GEOMETRY is implemented on top of BLOB.
       BIT may store its data among NULL bits, convert as well.
     */
      switch (f->type()) {
        case MYSQL_TYPE_BLOB:
        case MYSQL_TYPE_VARCHAR:
        case MYSQL_TYPE_GEOMETRY:
        case MYSQL_TYPE_BIT:
        {
          flush_checksum(&row_crc, &checksum_start, &checksum_length);
          String tmp;
          f->val_str(&tmp);
          row_crc= my_checksum(row_crc, (uchar*) tmp.ptr(), tmp.length());
          break;
        }
        default:
          if (!checksum_start)
            checksum_start= f->ptr;
          DBUG_ASSERT(checksum_start + checksum_length == f->ptr);
          checksum_length+= f->pack_length();
          break;
      }
    }
    flush_checksum(&row_crc, &checksum_start, &checksum_length);

    stats.checksum+= row_crc;
  }
  ha_rnd_end();
  return error == HA_ERR_END_OF_FILE ? 0 : error;
}