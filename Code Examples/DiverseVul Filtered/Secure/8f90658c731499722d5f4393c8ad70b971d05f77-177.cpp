int RGWHandler_Auth_S3::init(rgw::sal::RGWRadosStore *store, struct req_state *state,
                             rgw::io::BasicClient *cio)
{
  int ret = RGWHandler_REST_S3::init_from_header(state, RGW_FORMAT_JSON,
						     true);
  if (ret < 0)
    return ret;

  return RGWHandler_REST::init(store, state, cio);
}