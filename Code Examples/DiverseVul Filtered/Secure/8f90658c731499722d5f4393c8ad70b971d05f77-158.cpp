static void dump_usage_bucket_info(Formatter *formatter, const std::string& name, const cls_user_bucket_entry& entry)
{
  formatter->open_object_section("Entry");
  encode_json("Bucket", name, formatter);
  encode_json("Bytes", entry.size, formatter);
  encode_json("Bytes_Rounded", entry.size_rounded, formatter);
  formatter->close_section(); // entry
}