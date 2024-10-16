CtPtr ProtocolV1::send_connect_message() {
  state = CONNECTING_SEND_CONNECT_MSG;

  ldout(cct, 20) << __func__ << dendl;

  if (!authorizer) {
    authorizer = messenger->ms_deliver_get_authorizer(connection->peer_type);
  }

  ceph_msg_connect connect;
  connect.features = connection->policy.features_supported;
  connect.host_type = messenger->get_myname().type();
  connect.global_seq = global_seq;
  connect.connect_seq = connect_seq;
  connect.protocol_version =
      messenger->get_proto_version(connection->peer_type, true);
  connect.authorizer_protocol = authorizer ? authorizer->protocol : 0;
  connect.authorizer_len = authorizer ? authorizer->bl.length() : 0;

  if (authorizer) {
    ldout(cct, 10) << __func__
                   << " connect_msg.authorizer_len=" << connect.authorizer_len
                   << " protocol=" << connect.authorizer_protocol << dendl;
  }

  connect.flags = 0;
  if (connection->policy.lossy) {
    connect.flags |=
        CEPH_MSG_CONNECT_LOSSY;  // this is fyi, actually, server decides!
  }

  bufferlist bl;
  bl.append((char *)&connect, sizeof(connect));
  if (authorizer) {
    bl.append(authorizer->bl.c_str(), authorizer->bl.length());
  }

  ldout(cct, 10) << __func__ << " connect sending gseq=" << global_seq
                 << " cseq=" << connect_seq
                 << " proto=" << connect.protocol_version << dendl;

  return WRITE(bl, handle_connect_message_write);
}