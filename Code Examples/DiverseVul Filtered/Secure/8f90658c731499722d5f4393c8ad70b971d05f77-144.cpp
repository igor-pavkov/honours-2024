void RGWPostObj_ObjStore_S3::send_response()
{
  if (op_ret == 0 && parts.count("success_action_redirect")) {
    string redirect;

    part_str(parts, "success_action_redirect", &redirect);

    string tenant;
    string bucket;
    string key;
    string etag_str = "\"";

    etag_str.append(etag);
    etag_str.append("\"");

    string etag_url;

    url_encode(s->bucket_tenant, tenant); /* surely overkill, but cheap */
    url_encode(s->bucket_name, bucket);
    url_encode(s->object.name, key);
    url_encode(etag_str, etag_url);

    if (!s->bucket_tenant.empty()) {
      /*
       * What we really would like is to quaily the bucket name, so
       * that the client could simply copy it and paste into next request.
       * Unfortunately, in S3 we cannot know if the client will decide
       * to come through DNS, with "bucket.tenant" sytanx, or through
       * URL with "tenant\bucket" syntax. Therefore, we provide the
       * tenant separately.
       */
      redirect.append("?tenant=");
      redirect.append(tenant);
      redirect.append("&bucket=");
      redirect.append(bucket);
    } else {
      redirect.append("?bucket=");
      redirect.append(bucket);
    }
    redirect.append("&key=");
    redirect.append(key);
    redirect.append("&etag=");
    redirect.append(etag_url);

    int r = check_utf8(redirect.c_str(), redirect.size());
    if (r < 0) {
      op_ret = r;
      goto done;
    }
    dump_redirect(s, redirect);
    op_ret = STATUS_REDIRECT;
  } else if (op_ret == 0 && parts.count("success_action_status")) {
    string status_string;
    uint32_t status_int;

    part_str(parts, "success_action_status", &status_string);

    int r = stringtoul(status_string, &status_int);
    if (r < 0) {
      op_ret = r;
      goto done;
    }

    switch (status_int) {
      case 200:
	break;
      case 201:
	op_ret = STATUS_CREATED;
	break;
      default:
	op_ret = STATUS_NO_CONTENT;
	break;
    }
  } else if (! op_ret) {
    op_ret = STATUS_NO_CONTENT;
  }

done:
  if (op_ret == STATUS_CREATED) {
    for (auto &it : crypt_http_responses)
      dump_header(s, it.first, it.second);
    s->formatter->open_object_section("PostResponse");
    std::string base_uri = compute_domain_uri(s);
    if (!s->bucket_tenant.empty()){
      s->formatter->dump_format("Location", "%s/%s:%s/%s",
                                base_uri.c_str(),
                                url_encode(s->bucket_tenant).c_str(),
                                url_encode(s->bucket_name).c_str(),
                                url_encode(s->object.name).c_str());
      s->formatter->dump_string("Tenant", s->bucket_tenant);
    } else {
      s->formatter->dump_format("Location", "%s/%s/%s",
                                base_uri.c_str(),
                                url_encode(s->bucket_name).c_str(),
                                url_encode(s->object.name).c_str());
    }
    s->formatter->dump_string("Bucket", s->bucket_name);
    s->formatter->dump_string("Key", s->object.name);
    s->formatter->dump_string("ETag", etag);
    s->formatter->close_section();
  }
  s->err.message = err_msg;
  set_req_state_err(s, op_ret);
  dump_errno(s);
  if (op_ret >= 0) {
    dump_content_length(s, s->formatter->get_len());
  }
  end_header(s, this);
  if (op_ret != STATUS_CREATED)
    return;

  rgw_flush_formatter_and_reset(s, s->formatter);
}