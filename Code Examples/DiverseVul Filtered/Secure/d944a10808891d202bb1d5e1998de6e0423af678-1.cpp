uint64 CSteamNetworkListenSocketDirectUDP::GenerateChallenge( uint16 nTime, const netadr_t &adr ) const
{
	#pragma pack(push,1)
	struct
	{
		uint16 nTime;
		uint16 nPort;
		uint8 ipv6[16];
	} data;
	#pragma pack(pop)
	data.nTime = nTime;
	data.nPort = adr.GetPort();
	adr.GetIPV6( data.ipv6 );
	uint64 nChallenge = siphash( (const uint8_t *)&data, sizeof(data), m_argbChallengeSecret );
	return ( nChallenge & 0xffffffffffff0000ull ) | nTime;
}