int RGWHandler_REST_S3Website::error_handler(int err_no,
					    string* error_content) {
  int new_err_no = -1;
  rgw_http_errors::const_iterator r = rgw_http_s3_errors.find(err_no > 0 ? err_no : -err_no);
  int http_error_code = -1;

  if (r != rgw_http_s3_errors.end()) {
    http_error_code = r->second.first;
  }
  ldpp_dout(s, 10) << "RGWHandler_REST_S3Website::error_handler err_no=" << err_no << " http_ret=" << http_error_code << dendl;

  RGWBWRoutingRule rrule;
  bool should_redirect =
    s->bucket_info.website_conf.should_redirect(original_object_name,
                                                http_error_code, &rrule);

  if (should_redirect) {
    const string& hostname = s->info.env->get("HTTP_HOST", "");
    const string& protocol =
      (s->info.env->get("SERVER_PORT_SECURE") ? "https" : "http");
    int redirect_code = 0;
    rrule.apply_rule(protocol, hostname, original_object_name,
                     &s->redirect, &redirect_code);
    // Apply a custom HTTP response code
    if (redirect_code > 0)
      s->err.http_ret = redirect_code; // Apply a custom HTTP response code
    ldpp_dout(s, 10) << "error handler redirect code=" << redirect_code
		      << " proto+host:" << protocol << "://" << hostname
		      << " -> " << s->redirect << dendl;
    return -ERR_WEBSITE_REDIRECT;
  } else if (err_no == -ERR_WEBSITE_REDIRECT) {
    // Do nothing here, this redirect will be handled in abort_early's ERR_WEBSITE_REDIRECT block
    // Do NOT fire the ErrorDoc handler
  } else if (!s->bucket_info.website_conf.error_doc.empty()) {
    /* This serves an entire page!
       On success, it will return zero, and no further content should be sent to the socket
       On failure, we need the double-error handler
     */
    new_err_no = RGWHandler_REST_S3Website::serve_errordoc(http_error_code, s->bucket_info.website_conf.error_doc);
    if (new_err_no && new_err_no != -1) {
      err_no = new_err_no;
    }
  } else {
    ldpp_dout(s, 20) << "No special error handling today!" << dendl;
  }

  return err_no;
}