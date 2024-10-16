int RGWPostObj_ObjStore_S3::get_params()
{
  op_ret = RGWPostObj_ObjStore::get_params();
  if (op_ret < 0) {
    return op_ret;
  }

  map_qs_metadata(s);

  ldout(s->cct, 20) << "adding bucket to policy env: " << s->bucket.name
		    << dendl;
  env.add_var("bucket", s->bucket.name);

  bool done;
  do {
    struct post_form_part part;
    int r = read_form_part_header(&part, done);
    if (r < 0)
      return r;

    if (s->cct->_conf->subsys.should_gather<ceph_subsys_rgw, 20>()) {
      ldout(s->cct, 20) << "read part header -- part.name="
                        << part.name << dendl;

      for (const auto& pair : part.fields) {
        ldout(s->cct, 20) << "field.name=" << pair.first << dendl;
        ldout(s->cct, 20) << "field.val=" << pair.second.val << dendl;
        ldout(s->cct, 20) << "field.params:" << dendl;

        for (const auto& param_pair : pair.second.params) {
          ldout(s->cct, 20) << " " << param_pair.first
                            << " -> " << param_pair.second << dendl;
        }
      }
    }

    if (done) { /* unexpected here */
      err_msg = "Malformed request";
      return -EINVAL;
    }

    if (stringcasecmp(part.name, "file") == 0) { /* beginning of data transfer */
      struct post_part_field& field = part.fields["Content-Disposition"];
      map<string, string>::iterator iter = field.params.find("filename");
      if (iter != field.params.end()) {
	filename = iter->second;
      }
      parts[part.name] = part;
      break;
    }

    bool boundary;
    uint64_t chunk_size = s->cct->_conf->rgw_max_chunk_size;
    r = read_data(part.data, chunk_size, boundary, done);
    if (r < 0 || !boundary) {
      err_msg = "Couldn't find boundary";
      return -EINVAL;
    }
    parts[part.name] = part;
    string part_str(part.data.c_str(), part.data.length());
    env.add_var(part.name, part_str);
  } while (!done);

  string object_str;
  if (!part_str(parts, "key", &object_str)) {
    err_msg = "Key not specified";
    return -EINVAL;
  }

  s->object = rgw_obj_key(object_str);

  rebuild_key(s->object.name);

  if (s->object.empty()) {
    err_msg = "Empty object name";
    return -EINVAL;
  }

  env.add_var("key", s->object.name);

  part_str(parts, "Content-Type", &content_type);

  /* AWS permits POST without Content-Type: http://tracker.ceph.com/issues/20201 */
  if (! content_type.empty()) {
    env.add_var("Content-Type", content_type);
  }

  map<string, struct post_form_part, ltstr_nocase>::iterator piter =
    parts.upper_bound(RGW_AMZ_META_PREFIX);
  for (; piter != parts.end(); ++piter) {
    string n = piter->first;
    if (strncasecmp(n.c_str(), RGW_AMZ_META_PREFIX,
		    sizeof(RGW_AMZ_META_PREFIX) - 1) != 0)
      break;

    string attr_name = RGW_ATTR_PREFIX;
    attr_name.append(n);

    /* need to null terminate it */
    bufferlist& data = piter->second.data;
    string str = string(data.c_str(), data.length());

    bufferlist attr_bl;
    attr_bl.append(str.c_str(), str.size() + 1);

    attrs[attr_name] = attr_bl;
  }
  // TODO: refactor this and the above loop to share code
  piter = parts.find(RGW_AMZ_WEBSITE_REDIRECT_LOCATION);
  if (piter != parts.end()) {
    string n = piter->first;
    string attr_name = RGW_ATTR_PREFIX;
    attr_name.append(n);
    /* need to null terminate it */
    bufferlist& data = piter->second.data;
    string str = string(data.c_str(), data.length());

    bufferlist attr_bl;
    attr_bl.append(str.c_str(), str.size() + 1);

    attrs[attr_name] = attr_bl;
  }

  int r = get_policy();
  if (r < 0)
    return r;

  r = get_tags();
  if (r < 0)
    return r;


  min_len = post_policy.min_length;
  max_len = post_policy.max_length;



  return 0;
}