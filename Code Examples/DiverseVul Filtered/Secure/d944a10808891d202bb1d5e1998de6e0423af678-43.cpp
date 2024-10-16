CConnectionTransportUDP::~CConnectionTransportUDP()
{
	Assert( !m_pSocket ); // Use TransportDestroySelfNow!
}