double handler::keyread_time(uint index, uint ranges, ha_rows rows)
{
  DBUG_ASSERT(ranges == 0 || ranges == 1);
  size_t len= table->key_info[index].key_length + ref_length;
  if (index == table->s->primary_key && table->file->primary_key_is_clustered())
    len= table->s->stored_rec_length;
  double cost= (double)rows*len/(stats.block_size+1)*IDX_BLOCK_COPY_COST;
  if (ranges)
  {
    uint keys_per_block= (uint) (stats.block_size/2.0/len+1);
    ulonglong blocks= !rows ? 0 : (rows-1) / keys_per_block + 1;
    cost+= blocks;
  }
  return cost;
}