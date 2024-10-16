static void dump_usage_categories_info(Formatter *formatter, const rgw_usage_log_entry& entry, map<string, bool> *categories)
{
  formatter->open_array_section("categories");
  map<string, rgw_usage_data>::const_iterator uiter;
  for (uiter = entry.usage_map.begin(); uiter != entry.usage_map.end(); ++uiter) {
    if (categories && !categories->empty() && !categories->count(uiter->first))
      continue;
    const rgw_usage_data& usage = uiter->second;
    formatter->open_object_section("Entry");
    encode_json("Category", uiter->first, formatter);
    encode_json("BytesSent", usage.bytes_sent, formatter);
    encode_json("BytesReceived", usage.bytes_received, formatter);
    encode_json("Ops", usage.ops, formatter);
    encode_json("SuccessfulOps", usage.successful_ops, formatter);
    formatter->close_section(); // Entry
  }
  formatter->close_section(); // Category
}