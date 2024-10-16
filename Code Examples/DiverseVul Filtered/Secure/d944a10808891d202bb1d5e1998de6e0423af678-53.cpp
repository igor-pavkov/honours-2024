void CSteamNetworkListenSocketDirectUDP::SendMsg( uint8 nMsgID, const google::protobuf::MessageLite &msg, const netadr_t &adrTo )
{
	if ( !m_pSock )
	{
		Assert( false );
		return;
	}

	uint8 pkt[ k_cbSteamNetworkingSocketsMaxUDPMsgLen ];
	pkt[0] = nMsgID;
	int cbPkt = ProtoMsgByteSize( msg )+1;
	if ( cbPkt > sizeof(pkt) )
	{
		AssertMsg3( false, "Msg type %d is %d bytes, larger than MTU of %d bytes", int( nMsgID ), int( cbPkt ), (int)sizeof(pkt) );
		return;
	}
	uint8 *pEnd = msg.SerializeWithCachedSizesToArray( pkt+1 );
	Assert( cbPkt == pEnd - pkt );

	// Send the reply
	m_pSock->BSendRawPacket( pkt, cbPkt, adrTo );
}