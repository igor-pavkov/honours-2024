EUnsignedCert CSteamNetworkConnectionUDP::AllowRemoteUnsignedCert()
{
	// NOTE: No special override for localhost.
	// Should we add a seperate convar for this?
	// For the CSteamNetworkConnectionlocalhostLoopback connection,
	// we know both ends are us.  but if they are just connecting to
	// 127.0.0.1, it's not clear that we should handle this any
	// differently from any other connection

	// Enabled by convar?
	int nAllow = m_connectionConfig.m_IP_AllowWithoutAuth.Get();
	if ( nAllow > 1 )
		return k_EUnsignedCert_Allow;
	if ( nAllow == 1 )
		return k_EUnsignedCert_AllowWarn;

	// Lock it down
	return k_EUnsignedCert_Disallow;
}