void RGWGetUsage_ObjStore_S3::send_response()
{
  if (op_ret < 0)
    set_req_state_err(s, op_ret);
  dump_errno(s);

  // Explicitly use chunked transfer encoding so that we can stream the result
  // to the user without having to wait for the full length of it.
  end_header(s, this, "application/xml", CHUNKED_TRANSFER_ENCODING);
  dump_start(s);
  if (op_ret < 0)
    return;

  Formatter *formatter = s->formatter;
  string last_owner;
  bool user_section_open = false;
  
  formatter->open_object_section("Usage");
  if (show_log_entries) {
    formatter->open_array_section("Entries");
  }
  map<rgw_user_bucket, rgw_usage_log_entry>::iterator iter;
  for (iter = usage.begin(); iter != usage.end(); ++iter) {
    const rgw_user_bucket& ub = iter->first;
    const rgw_usage_log_entry& entry = iter->second;

    if (show_log_entries) {
      if (ub.user.compare(last_owner) != 0) {
        if (user_section_open) {
          formatter->close_section();
          formatter->close_section();
        }
        formatter->open_object_section("User");
        formatter->dump_string("Owner", ub.user);
        formatter->open_array_section("Buckets");
        user_section_open = true;
        last_owner = ub.user;
      }
      formatter->open_object_section("Bucket");
      formatter->dump_string("Bucket", ub.bucket);
      utime_t ut(entry.epoch, 0);
      ut.gmtime(formatter->dump_stream("Time"));
      formatter->dump_int("Epoch", entry.epoch);
      dump_usage_categories_info(formatter, entry, &categories);
      formatter->close_section(); // bucket
    }

    summary_map[ub.user].aggregate(entry, &categories);
  }

  if (show_log_entries) {
     if (user_section_open) {
       formatter->close_section(); // buckets
       formatter->close_section(); //user
     }
     formatter->close_section(); // entries
   }

   if (show_log_sum) {
     formatter->open_array_section("Summary");
     map<string, rgw_usage_log_entry>::iterator siter;
     for (siter = summary_map.begin(); siter != summary_map.end(); ++siter) {
       const rgw_usage_log_entry& entry = siter->second;
       formatter->open_object_section("User");
       formatter->dump_string("User", siter->first);
       dump_usage_categories_info(formatter, entry, &categories);
       rgw_usage_data total_usage;
       entry.sum(total_usage, categories);
       formatter->open_object_section("Total");
       encode_json("BytesSent", total_usage.bytes_sent, formatter);
       encode_json("BytesReceived", total_usage.bytes_received, formatter);
       encode_json("Ops", total_usage.ops, formatter);
       encode_json("SuccessfulOps", total_usage.successful_ops, formatter);
       formatter->close_section(); // total
       formatter->close_section(); // user
     }

     if (s->cct->_conf->rgw_rest_getusage_op_compat) {
       formatter->open_object_section("Stats");
     }

     encode_json("TotalBytes", stats.size, formatter);
     encode_json("TotalBytesRounded", stats.size_rounded, formatter);
     encode_json("TotalEntries", stats.num_objects, formatter);

     if (s->cct->_conf->rgw_rest_getusage_op_compat) {
       formatter->close_section(); //Stats
     }

     formatter->close_section(); // summary
   }

  formatter->open_array_section("CapacityUsed");
  formatter->open_object_section("User");
  formatter->open_array_section("Buckets");
  for (const auto& biter : buckets_usage) {
    const cls_user_bucket_entry& entry = biter.second;
    dump_usage_bucket_info(formatter, biter.first, entry);
  }
  formatter->close_section(); // Buckets
  formatter->close_section(); // User
  formatter->close_section(); // CapacityUsed

  formatter->close_section(); // usage
  rgw_flush_formatter_and_reset(s, s->formatter);
}