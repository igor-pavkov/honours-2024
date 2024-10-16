static void dump_usage_bucket_info(Formatter *formatter, const std::string& name, const cls_user_bucket_entry& entry)
{
  formatter->open_object_section("Entry");
  formatter->dump_string("Bucket", name);
  formatter->dump_int("Bytes", entry.size);
  formatter->dump_int("Bytes_Rounded", entry.size_rounded);
  formatter->close_section(); // entry
}