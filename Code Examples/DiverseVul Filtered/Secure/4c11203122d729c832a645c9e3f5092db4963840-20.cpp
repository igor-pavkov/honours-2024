void ProtocolV1::prepare_send_message(uint64_t features, Message *m,
                                      bufferlist &bl) {
  ldout(cct, 20) << __func__ << " m " << *m << dendl;

  // associate message with Connection (for benefit of encode_payload)
  if (m->empty_payload()) {
    ldout(cct, 20) << __func__ << " encoding features " << features << " " << m
                   << " " << *m << dendl;
  } else {
    ldout(cct, 20) << __func__ << " half-reencoding features " << features
                   << " " << m << " " << *m << dendl;
  }

  // encode and copy out of *m
  m->encode(features, messenger->crcflags);

  bl.append(m->get_payload());
  bl.append(m->get_middle());
  bl.append(m->get_data());
}