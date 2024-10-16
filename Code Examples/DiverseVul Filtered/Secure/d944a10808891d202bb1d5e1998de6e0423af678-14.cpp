CSteamNetworkListenSocketDirectUDP::~CSteamNetworkListenSocketDirectUDP()
{
	// Clean up socket, if any
	if ( m_pSock )
	{
		delete m_pSock;
		m_pSock = nullptr;
	}
}