inline uint16 GetChallengeTime( SteamNetworkingMicroseconds usecNow )
{
	return uint16( usecNow >> 20 );
}