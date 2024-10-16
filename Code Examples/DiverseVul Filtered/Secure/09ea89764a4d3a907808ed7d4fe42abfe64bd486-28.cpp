find_intro_point(origin_circuit_t *circ)
{
  const char *serviceid;
  rend_service_t *service = NULL;

  tor_assert(TO_CIRCUIT(circ)->purpose == CIRCUIT_PURPOSE_S_ESTABLISH_INTRO ||
             TO_CIRCUIT(circ)->purpose == CIRCUIT_PURPOSE_S_INTRO);
  tor_assert(circ->rend_data);
  serviceid = rend_data_get_address(circ->rend_data);

  SMARTLIST_FOREACH(rend_service_list, rend_service_t *, s,
    if (tor_memeq(s->service_id, serviceid, REND_SERVICE_ID_LEN_BASE32)) {
      service = s;
      break;
    });

  if (service == NULL) return NULL;

  SMARTLIST_FOREACH(service->intro_nodes, rend_intro_point_t *, intro_point,
    if (crypto_pk_eq_keys(intro_point->intro_key, circ->intro_key)) {
      return intro_point;
    });

  return NULL;
}