int RGWHandler_REST_S3Website::init(rgw::sal::RGWRadosStore *store, req_state *s,
                                    rgw::io::BasicClient* cio)
{
  // save the original object name before retarget() replaces it with the
  // result of get_effective_key(). the error_handler() needs the original
  // object name for redirect handling
  original_object_name = s->object.name;

  return RGWHandler_REST_S3::init(store, s, cio);
}