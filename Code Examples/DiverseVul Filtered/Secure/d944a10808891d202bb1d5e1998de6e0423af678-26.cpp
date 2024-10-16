bool CConnectionTransportUDP::BCanSendEndToEndData() const
{
	return m_pSocket != nullptr;
}