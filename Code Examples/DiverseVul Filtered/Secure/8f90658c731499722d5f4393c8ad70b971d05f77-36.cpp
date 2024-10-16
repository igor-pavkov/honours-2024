void dump_bucket(struct req_state *s, rgw::sal::RGWBucket& obj)
{
  s->formatter->open_object_section("Bucket");
  s->formatter->dump_string("Name", obj.get_name());
  dump_time(s, "CreationDate", &obj.get_creation_time());
  s->formatter->close_section();
}