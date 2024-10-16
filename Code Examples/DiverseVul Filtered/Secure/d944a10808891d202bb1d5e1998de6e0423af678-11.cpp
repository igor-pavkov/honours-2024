bool CConnectionTransportUDP::BCanSendEndToEndConnectRequest() const
{
	return m_pSocket != nullptr;
}