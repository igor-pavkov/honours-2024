CSteamNetworkConnectionlocalhostLoopback::CSteamNetworkConnectionlocalhostLoopback( CSteamNetworkingSockets *pSteamNetworkingSocketsInterface, const SteamNetworkingIdentity &identity )
: CSteamNetworkConnectionUDP( pSteamNetworkingSocketsInterface )
{
	m_identityLocal = identity;
}