PJ_DEF(void) pjmedia_rtcp_tx_rtp(pjmedia_rtcp_session *sess, 
				 unsigned bytes_payload_size)
{
    /* Update statistics */
    sess->stat.tx.pkt++;
    sess->stat.tx.bytes += bytes_payload_size;
}