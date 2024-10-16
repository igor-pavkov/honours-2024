void CConnectionTransportUDPBase::SendEndToEndStatsMsg( EStatsReplyRequest eRequest, SteamNetworkingMicroseconds usecNow, const char *pszReason )
{
	UDPSendPacketContext_t ctx( usecNow, pszReason );
	ctx.Populate( sizeof(UDPDataMsgHdr), eRequest, this );

	// Send a data packet (maybe containing ordinary data), with this piggy backed on top of it
	m_connection.SNP_SendPacket( this, ctx );
}