bool CConnectionTransportUDP::SendPacketGather( int nChunks, const iovec *pChunks, int cbSendTotal )
{
	// Safety
	if ( !m_pSocket )
	{
		AssertMsg( false, "Attemt to send packet, but socket has been closed!" );
		return false;
	}

	// Update stats
	m_connection.m_statsEndToEnd.TrackSentPacket( cbSendTotal );

	// Hand over to operating system
	return m_pSocket->BSendRawPacketGather( nChunks, pChunks );
}