void CSteamNetworkConnectionUDP::GetConnectionTypeDescription( ConnectionTypeDescription_t &szDescription ) const
{
	char szAddr[ 64 ];
	if ( Transport() && Transport()->m_pSocket )
	{
		SteamNetworkingIPAddr adrRemote;
		NetAdrToSteamNetworkingIPAddr( adrRemote, Transport()->m_pSocket->GetRemoteHostAddr() );
		adrRemote.ToString( szAddr, sizeof(szAddr), true );
		if (
			m_identityRemote.IsLocalHost()
			|| ( m_identityRemote.m_eType == k_ESteamNetworkingIdentityType_IPAddress && adrRemote == m_identityRemote.m_ip )
		) {
			V_sprintf_safe( szDescription, "UDP %s", szAddr );
			return;
		}
	}
	else
	{
		V_strcpy_safe( szAddr, "???" );
	}

	SteamNetworkingIdentityRender sIdentity( m_identityRemote );

	V_sprintf_safe( szDescription, "UDP %s@%s", sIdentity.c_str(), szAddr );
}