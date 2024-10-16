static void dump_usage_categories_info(Formatter *formatter, const rgw_usage_log_entry& entry, map<string, bool> *categories)
{
  formatter->open_array_section("categories");
  map<string, rgw_usage_data>::const_iterator uiter;
  for (uiter = entry.usage_map.begin(); uiter != entry.usage_map.end(); ++uiter) {
    if (categories && !categories->empty() && !categories->count(uiter->first))
      continue;
    const rgw_usage_data& usage = uiter->second;
    formatter->open_object_section("Entry");
    formatter->dump_string("Category", uiter->first);
    formatter->dump_int("BytesSent", usage.bytes_sent);
    formatter->dump_int("BytesReceived", usage.bytes_received);
    formatter->dump_int("Ops", usage.ops);
    formatter->dump_int("SuccessfulOps", usage.successful_ops);
    formatter->close_section(); // Entry
  }
  formatter->close_section(); // Category
}