static int StreamTcpPacketStateLastAck(ThreadVars *tv, Packet *p,
                        StreamTcpThread *stt, TcpSession *ssn, PacketQueue *pq)
{
    if (ssn == NULL)
        return -1;

    if (p->tcph->th_flags & TH_RST) {
        if (!StreamTcpValidateRst(ssn, p))
            return -1;

        StreamTcpPacketSetState(p, ssn, TCP_CLOSED);
        SCLogDebug("ssn %p: Reset received state changed to TCP_CLOSED",
                ssn);

        if (PKT_IS_TOSERVER(p)) {
            if ((p->tcph->th_flags & TH_ACK) && StreamTcpValidateAck(ssn, &ssn->server, p) == 0)
                StreamTcpUpdateLastAck(ssn, &ssn->server,
                        StreamTcpResetGetMaxAck(&ssn->server, TCP_GET_ACK(p)));

            StreamTcpUpdateLastAck(ssn, &ssn->client,
                    StreamTcpResetGetMaxAck(&ssn->client, TCP_GET_SEQ(p)));

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->client, p, pq);
        } else {
            if ((p->tcph->th_flags & TH_ACK) && StreamTcpValidateAck(ssn, &ssn->client, p) == 0)
                StreamTcpUpdateLastAck(ssn, &ssn->client,
                        StreamTcpResetGetMaxAck(&ssn->client, TCP_GET_ACK(p)));

            StreamTcpUpdateLastAck(ssn, &ssn->server,
                    StreamTcpResetGetMaxAck(&ssn->server, TCP_GET_SEQ(p)));

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->server, p, pq);
        }

    } else if (p->tcph->th_flags & TH_FIN) {
        /** \todo */
        SCLogDebug("ssn (%p): FIN pkt on LastAck", ssn);

    } else if (p->tcph->th_flags & TH_SYN) {
        SCLogDebug("ssn (%p): SYN pkt on LastAck", ssn);
        StreamTcpSetEvent(p, STREAM_SHUTDOWN_SYN_RESEND);
        return -1;

    } else if (p->tcph->th_flags & TH_ACK) {
        if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
            if (!StreamTcpValidateTimestamp(ssn, p))
                return -1;
        }

        if (PKT_IS_TOSERVER(p)) {
            SCLogDebug("ssn %p: pkt (%" PRIu32 ") is to server: SEQ "
                    "%" PRIu32 ", ACK %" PRIu32 "", ssn, p->payload_len,
                    TCP_GET_SEQ(p), TCP_GET_ACK(p));

            int retransmission = 0;
            if (StreamTcpPacketIsRetransmission(&ssn->client, p)) {
                SCLogDebug("ssn %p: packet is retransmission", ssn);
                retransmission = 1;
            }

            if (StreamTcpValidateAck(ssn, &ssn->server, p) == -1) {
                SCLogDebug("ssn %p: rejecting because of invalid ack value", ssn);
                StreamTcpSetEvent(p, STREAM_LASTACK_INVALID_ACK);
                SCReturnInt(-1);
            }

            if (!retransmission) {
                if (SEQ_LT(TCP_GET_SEQ(p), ssn->client.next_seq)) {
                    SCLogDebug("ssn %p: not updating state as packet is before next_seq", ssn);
                } else if (TCP_GET_SEQ(p) != ssn->client.next_seq && TCP_GET_SEQ(p) != ssn->client.next_seq + 1) {
                    SCLogDebug("ssn %p: -> SEQ mismatch, packet SEQ %" PRIu32 ""
                            " != %" PRIu32 " from stream", ssn,
                            TCP_GET_SEQ(p), ssn->client.next_seq);
                    StreamTcpSetEvent(p, STREAM_LASTACK_ACK_WRONG_SEQ);
                    return -1;
                } else {
                    StreamTcpPacketSetState(p, ssn, TCP_CLOSED);
                    SCLogDebug("ssn %p: state changed to TCP_CLOSED", ssn);

                }
                ssn->server.window = TCP_GET_WINDOW(p) << ssn->server.wscale;
            }

            StreamTcpUpdateLastAck(ssn, &ssn->server, TCP_GET_ACK(p));

            if (ssn->flags & STREAMTCP_FLAG_TIMESTAMP) {
                StreamTcpHandleTimestamp(ssn, p);
            }

            /* Update the next_seq, in case if we have missed the client
               packet and server has already received and acked it */
            if (SEQ_LT(ssn->server.next_seq, TCP_GET_ACK(p)))
                ssn->server.next_seq = TCP_GET_ACK(p);

            StreamTcpReassembleHandleSegment(tv, stt->ra_ctx, ssn,
                    &ssn->client, p, pq);
            SCLogDebug("ssn %p: =+ next SEQ %" PRIu32 ", last ACK "
                    "%" PRIu32 "", ssn, ssn->client.next_seq,
                    ssn->server.last_ack);
        }
    } else {
        SCLogDebug("ssn %p: default case", ssn);
    }

    return 0;
}