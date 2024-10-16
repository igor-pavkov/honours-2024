CtPtr ProtocolV1::handle_connect_reply_auth(char *buffer, int r) {
  ldout(cct, 20) << __func__ << " r=" << r << dendl;

  if (r < 0) {
    ldout(cct, 1) << __func__ << " read connect reply authorizer failed"
                  << dendl;
    return _fault();
  }

  bufferlist authorizer_reply;
  authorizer_reply.append(buffer, connect_reply.authorizer_len);

  if (connect_reply.tag == CEPH_MSGR_TAG_CHALLENGE_AUTHORIZER) {
    ldout(cct, 10) << __func__ << " connect got auth challenge" << dendl;
    authorizer->add_challenge(cct, authorizer_reply);
    return CONTINUE(send_connect_message);
  }

  auto iter = authorizer_reply.cbegin();
  if (authorizer && !authorizer->verify_reply(iter,
					      nullptr /* connection_secret */)) {
    ldout(cct, 0) << __func__ << " failed verifying authorize reply" << dendl;
    return _fault();
  }

  return handle_connect_reply_2();
}