bool BCheckRateLimitReportBadPacket( SteamNetworkingMicroseconds usecNow )
{
	static SteamNetworkingMicroseconds s_usecLastReport;
	if ( s_usecLastReport + k_nMillion*2 > usecNow )
		return false;
	s_usecLastReport = usecNow;
	return true;
}