bool CSteamNetworkListenSocketDirectUDP::APIGetAddress( SteamNetworkingIPAddr *pAddress )
{
	if ( !m_pSock )
	{
		Assert( false );
		return false;
	}

	const SteamNetworkingIPAddr *pBoundAddr = m_pSock->GetBoundAddr();
	if ( !pBoundAddr )
		return false;
	if ( pAddress )
		*pAddress = *pBoundAddr;
	return true;
}