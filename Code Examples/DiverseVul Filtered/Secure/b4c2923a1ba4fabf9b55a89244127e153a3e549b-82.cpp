static int r_jwe_set_enc_header(jwe_t * jwe, json_t * j_header) {
  int ret = RHN_OK;
  switch (jwe->enc) {
    case R_JWA_ENC_A128CBC:
      json_object_set_new(j_header, "enc", json_string("A128CBC-HS256"));
      break;
    case R_JWA_ENC_A192CBC:
      json_object_set_new(j_header, "enc", json_string("A192CBC-HS384"));
      break;
    case R_JWA_ENC_A256CBC:
      json_object_set_new(j_header, "enc", json_string("A256CBC-HS512"));
      break;
    case R_JWA_ENC_A128GCM:
      json_object_set_new(j_header, "enc", json_string("A128GCM"));
      break;
    case R_JWA_ENC_A192GCM:
      json_object_set_new(j_header, "enc", json_string("A192GCM"));
      break;
    case R_JWA_ENC_A256GCM:
      json_object_set_new(j_header, "enc", json_string("A256GCM"));
      break;
    default:
      ret = RHN_ERROR_PARAM;
      break;
  }
  return ret;
}