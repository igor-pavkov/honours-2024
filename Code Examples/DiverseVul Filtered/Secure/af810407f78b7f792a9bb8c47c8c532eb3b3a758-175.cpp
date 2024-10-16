void handler::get_dynamic_partition_info(PARTITION_STATS *stat_info,
                                         uint part_id)
{
  info(HA_STATUS_CONST | HA_STATUS_TIME | HA_STATUS_VARIABLE |
       HA_STATUS_NO_LOCK);
  stat_info->records=              stats.records;
  stat_info->mean_rec_length=      stats.mean_rec_length;
  stat_info->data_file_length=     stats.data_file_length;
  stat_info->max_data_file_length= stats.max_data_file_length;
  stat_info->index_file_length=    stats.index_file_length;
  stat_info->max_index_file_length=stats.max_index_file_length;
  stat_info->delete_length=        stats.delete_length;
  stat_info->create_time=          stats.create_time;
  stat_info->update_time=          stats.update_time;
  stat_info->check_time=           stats.check_time;
  stat_info->check_sum=            stats.checksum;
  stat_info->check_sum_null=       stats.checksum_null;
}