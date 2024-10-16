CConnectionTransportUDP::CConnectionTransportUDP( CSteamNetworkConnectionUDP &connection )
: CConnectionTransportUDPBase( connection )
, m_pSocket( nullptr )
{
}