static inline int StreamTcpStateDispatch(ThreadVars *tv, Packet *p,
        StreamTcpThread *stt, TcpSession *ssn, PacketQueue *pq,
        const uint8_t state)
{
    SCLogDebug("ssn: %p", ssn);
    switch (state) {
        case TCP_SYN_SENT:
            if (StreamTcpPacketStateSynSent(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_SYN_RECV:
            if (StreamTcpPacketStateSynRecv(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_ESTABLISHED:
            if (StreamTcpPacketStateEstablished(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_FIN_WAIT1:
            SCLogDebug("packet received on TCP_FIN_WAIT1 state");
            if (StreamTcpPacketStateFinWait1(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_FIN_WAIT2:
            SCLogDebug("packet received on TCP_FIN_WAIT2 state");
            if (StreamTcpPacketStateFinWait2(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_CLOSING:
            SCLogDebug("packet received on TCP_CLOSING state");
            if (StreamTcpPacketStateClosing(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_CLOSE_WAIT:
            SCLogDebug("packet received on TCP_CLOSE_WAIT state");
            if (StreamTcpPacketStateCloseWait(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_LAST_ACK:
            SCLogDebug("packet received on TCP_LAST_ACK state");
            if (StreamTcpPacketStateLastAck(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_TIME_WAIT:
            SCLogDebug("packet received on TCP_TIME_WAIT state");
            if (StreamTcpPacketStateTimeWait(tv, p, stt, ssn, pq)) {
                return -1;
            }
            break;
        case TCP_CLOSED:
            /* TCP session memory is not returned to pool until timeout. */
            SCLogDebug("packet received on closed state");

            if (StreamTcpPacketStateClosed(tv, p, stt, ssn, pq)) {
                return -1;
            }

            break;
        default:
            SCLogDebug("packet received on default state");
            break;
    }
    return 0;
}