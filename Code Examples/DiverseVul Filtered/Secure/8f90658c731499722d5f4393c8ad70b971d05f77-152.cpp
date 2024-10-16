int RGWSetRequestPayment_ObjStore_S3::get_params()
{
  const auto max_size = s->cct->_conf->rgw_max_put_param_size;

  int r = 0;
  std::tie(r, in_data) = rgw_rest_read_all_input(s, max_size, false);

  if (r < 0) {
    return r;
  }


  RGWSetRequestPaymentParser parser;

  if (!parser.init()) {
    ldpp_dout(this, 0) << "ERROR: failed to initialize parser" << dendl;
    return -EIO;
  }

  char* buf = in_data.c_str();
  if (!parser.parse(buf, in_data.length(), 1)) {
    ldpp_dout(this, 10) << "failed to parse data: " << buf << dendl;
    return -EINVAL;
  }

  return parser.get_request_payment_payer(&requester_pays);
}