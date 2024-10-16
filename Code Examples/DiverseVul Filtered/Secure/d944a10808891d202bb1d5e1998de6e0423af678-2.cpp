void CConnectionTransportUDP::TransportFreeResources()
{
	CConnectionTransport::TransportFreeResources();

	if ( m_pSocket )
	{
		m_pSocket->Close();
		m_pSocket = nullptr;
	}
}