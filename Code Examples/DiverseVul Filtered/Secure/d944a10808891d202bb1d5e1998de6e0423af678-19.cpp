void CConnectionTransportUDPBase::SendPaddedMsg( uint8 nMsgID, const google::protobuf::MessageLite &msg )
{

	uint8 pkt[ k_cbSteamNetworkingSocketsMaxUDPMsgLen ];
	V_memset( pkt, 0, sizeof(pkt) ); // don't send random bits from our process memory over the wire!
	UDPPaddedMessageHdr *hdr = (UDPPaddedMessageHdr *)pkt;
	int nMsgLength = ProtoMsgByteSize( msg );
	if ( nMsgLength + sizeof(*hdr) > k_cbSteamNetworkingSocketsMaxUDPMsgLen )
	{
		AssertMsg3( false, "Msg type %d is %d bytes, larger than MTU of %d bytes", int( nMsgID ), int( nMsgLength + sizeof(*hdr) ), (int)sizeof(pkt) );
		return;
	}
	hdr->m_nMsgID = nMsgID;
	hdr->m_nMsgLength = LittleWord( uint16( nMsgLength ) );
	uint8 *pEnd = msg.SerializeWithCachedSizesToArray( pkt + sizeof(*hdr) );
	int cbPkt = pEnd - pkt;
	Assert( cbPkt == int( sizeof(*hdr) + nMsgLength ) );
	cbPkt = MAX( cbPkt, k_cbSteamNetworkingMinPaddedPacketSize );

	SendPacket( pkt, cbPkt );
}