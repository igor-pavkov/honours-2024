EResult CSteamNetworkConnectionUDP::AcceptConnection( SteamNetworkingMicroseconds usecNow )
{
	if ( !Transport() )
	{
		AssertMsg( false, "Cannot acception UDP connection.  No transport?" );
		return k_EResultFail;
	}

	// Send the message
	Transport()->SendConnectOK( usecNow );

	// We are fully connected
	ConnectionState_Connected( usecNow );

	// OK
	return k_EResultOK;
}