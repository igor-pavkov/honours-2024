static int r_jwe_set_alg_header(jwe_t * jwe, json_t * j_header) {
  int ret = RHN_OK;
  switch (jwe->alg) {
    case R_JWA_ALG_NONE:
      json_object_set_new(j_header, "alg", json_string("none"));
      break;
    case R_JWA_ALG_RSA1_5:
      json_object_set_new(j_header, "alg", json_string("RSA1_5"));
      break;
    case R_JWA_ALG_RSA_OAEP:
      json_object_set_new(j_header, "alg", json_string("RSA-OAEP"));
      break;
    case R_JWA_ALG_RSA_OAEP_256:
      json_object_set_new(j_header, "alg", json_string("RSA-OAEP-256"));
      break;
    case R_JWA_ALG_A128KW:
      json_object_set_new(j_header, "alg", json_string("A128KW"));
      break;
    case R_JWA_ALG_A192KW:
      json_object_set_new(j_header, "alg", json_string("A192KW"));
      break;
    case R_JWA_ALG_A256KW:
      json_object_set_new(j_header, "alg", json_string("A256KW"));
      break;
    case R_JWA_ALG_DIR:
      json_object_set_new(j_header, "alg", json_string("dir"));
      break;
    case R_JWA_ALG_ECDH_ES:
      json_object_set_new(j_header, "alg", json_string("ECDH-ES"));
      break;
    case R_JWA_ALG_ECDH_ES_A128KW:
      json_object_set_new(j_header, "alg", json_string("ECDH-ES+A128KW"));
      break;
    case R_JWA_ALG_ECDH_ES_A192KW:
      json_object_set_new(j_header, "alg", json_string("ECDH-ES+A192KW"));
      break;
    case R_JWA_ALG_ECDH_ES_A256KW:
      json_object_set_new(j_header, "alg", json_string("ECDH-ES+A256KW"));
      break;
    case R_JWA_ALG_A128GCMKW:
      json_object_set_new(j_header, "alg", json_string("A128GCMKW"));
      break;
    case R_JWA_ALG_A192GCMKW:
      json_object_set_new(j_header, "alg", json_string("A192GCMKW"));
      break;
    case R_JWA_ALG_A256GCMKW:
      json_object_set_new(j_header, "alg", json_string("A256GCMKW"));
      break;
    case R_JWA_ALG_PBES2_H256:
      json_object_set_new(j_header, "alg", json_string("PBES2-HS256+A128KW"));
      break;
    case R_JWA_ALG_PBES2_H384:
      json_object_set_new(j_header, "alg", json_string("PBES2-HS384+A192KW"));
      break;
    case R_JWA_ALG_PBES2_H512:
      json_object_set_new(j_header, "alg", json_string("PBES2-HS512+A256KW"));
      break;
    default:
      ret = RHN_ERROR_PARAM;
      break;
  }
  return ret;
}