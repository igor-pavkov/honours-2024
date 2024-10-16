EUnsignedCert CSteamNetworkConnectionUDP::AllowLocalUnsignedCert()
{
	// Same logic actually applies for remote and local
	return AllowRemoteUnsignedCert();
}