static inline void map_qs_metadata(struct req_state* s)
{
  /* merge S3 valid user metadata from the query-string into
   * x_meta_map, which maps them to attributes */
  const auto& params = const_cast<RGWHTTPArgs&>(s->info.args).get_params();
  for (const auto& elt : params) {
    std::string k = boost::algorithm::to_lower_copy(elt.first);
    if (k.find("x-amz-meta-") == /* offset */ 0) {
      rgw_add_amz_meta_header(s->info.x_meta_map, k, elt.second);
    }
  }
}