void CSteamNetworkListenSocketDirectUDP::SendPaddedMsg( uint8 nMsgID, const google::protobuf::MessageLite &msg, const netadr_t adrTo )
{

	uint8 pkt[ k_cbSteamNetworkingSocketsMaxUDPMsgLen ];
	memset( pkt, 0, sizeof(pkt) ); // don't send random bits from our process memory over the wire!
	UDPPaddedMessageHdr *hdr = (UDPPaddedMessageHdr *)pkt;
	int nMsgLength = ProtoMsgByteSize( msg );
	hdr->m_nMsgID = nMsgID;
	hdr->m_nMsgLength = LittleWord( uint16( nMsgLength ) );
	uint8 *pEnd = msg.SerializeWithCachedSizesToArray( pkt + sizeof(*hdr) );
	int cbPkt = pEnd - pkt;
	Assert( cbPkt == int( sizeof(*hdr) + nMsgLength ) );
	cbPkt = MAX( cbPkt, k_cbSteamNetworkingMinPaddedPacketSize );

	m_pSock->BSendRawPacket( pkt, cbPkt, adrTo );
}