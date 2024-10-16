bool CConnectionTransportUDPBase::SendDataPacket( SteamNetworkingMicroseconds usecNow )
{
	// Populate context struct with any stats we want/need to send, and how much space we need to reserve for it
	UDPSendPacketContext_t ctx( usecNow, "data" );
	ctx.Populate( sizeof(UDPDataMsgHdr), k_EStatsReplyRequest_NothingToSend, this );

	// Send a packet
	return m_connection.SNP_SendPacket( this, ctx );
}