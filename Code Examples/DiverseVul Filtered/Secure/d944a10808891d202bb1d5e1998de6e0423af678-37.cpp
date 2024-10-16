int CConnectionTransportUDPBase::SendEncryptedDataChunk( const void *pChunk, int cbChunk, SendPacketContext_t &ctxBase )
{
	UDPSendPacketContext_t &ctx = static_cast<UDPSendPacketContext_t &>( ctxBase );

	uint8 pkt[ k_cbSteamNetworkingSocketsMaxUDPMsgLen ];
	UDPDataMsgHdr *hdr = (UDPDataMsgHdr *)pkt;
	hdr->m_unMsgFlags = 0x80;
	Assert( m_connection.m_unConnectionIDRemote != 0 );
	hdr->m_unToConnectionID = LittleDWord( m_connection.m_unConnectionIDRemote );
	hdr->m_unSeqNum = LittleWord( m_connection.m_statsEndToEnd.ConsumeSendPacketNumberAndGetWireFmt( ctx.m_usecNow ) );

	byte *p = (byte*)( hdr + 1 );

	// Check how much bigger we could grow the header
	// and still fit in a packet
	int cbHdrOutSpaceRemaining = pkt + sizeof(pkt) - p - cbChunk;
	if ( cbHdrOutSpaceRemaining < 0 )
	{
		AssertMsg( false, "MTU / header size problem!" );
		return 0;
	}

	// Try to trim stuff from blob, if it won't fit
	ctx.Trim( cbHdrOutSpaceRemaining );

	if ( ctx.Serialize( p ) )
	{
		// Update bookkeeping with the stuff we are actually sending
		TrackSentStats( ctx );

		// Mark header with the flag
		hdr->m_unMsgFlags |= hdr->kFlag_ProtobufBlob;
	}

	// !FIXME! Time since previous, for jitter measurement?

	// Use gather-based send.  This saves one memcpy of every payload
	iovec gather[2];
	gather[0].iov_base = pkt;
	gather[0].iov_len = p - pkt;
	gather[1].iov_base = const_cast<void*>( pChunk );
	gather[1].iov_len = cbChunk;

	int cbSend = gather[0].iov_len + gather[1].iov_len;
	Assert( cbSend <= sizeof(pkt) ); // Bug in the code above.  We should never "overflow" the packet.  (Ignoring the fact that we using a gather-based send.  The data could be tiny with a large header for piggy-backed stats.)

	// !FIXME! Should we track data payload separately?  Maybe we ought to track
	// *messages* instead of packets.

	// Send it
	if ( SendPacketGather( 2, gather, cbSend ) )
		return cbSend;
	return 0;
}