bool IsRouteToAddressProbablyLocal( netadr_t addr )
{

	// Assume that if we are able to send to any "reserved" route, that is is local.
	// Note that this will be true for VPNs, too!
	if ( addr.IsReservedAdr() )
		return true;

	// But other cases might also be local routes.  E.g. two boxes with public IPs.
	// Convert to sockaddr struct so we can ask the operating system
	addr.SetPort(0);
	sockaddr_storage sockaddrDest;
	addr.ToSockadr( &sockaddrDest );

	#ifdef _WINDOWS

		//
		// These functions were added with Vista, so load dynamically
		// in case
		//

		typedef
		DWORD
		(WINAPI *FnGetBestInterfaceEx)(
			struct sockaddr *pDestAddr,
			PDWORD           pdwBestIfIndex
			);
		typedef 
		NETIO_STATUS
		(NETIOAPI_API_*FnGetBestRoute2)(
			NET_LUID *InterfaceLuid,
			NET_IFINDEX InterfaceIndex,
			CONST SOCKADDR_INET *SourceAddress,
			CONST SOCKADDR_INET *DestinationAddress,
			ULONG AddressSortOptions,
			PMIB_IPFORWARD_ROW2 BestRoute,
			SOCKADDR_INET *BestSourceAddress
			);

		static HMODULE hModule = LoadLibraryA( "Iphlpapi.dll" );
		static FnGetBestInterfaceEx pGetBestInterfaceEx = hModule ? (FnGetBestInterfaceEx)GetProcAddress( hModule, "GetBestInterfaceEx" ) : nullptr;
		static FnGetBestRoute2 pGetBestRoute2 = hModule ? (FnGetBestRoute2)GetProcAddress( hModule, "GetBestRoute2" ) : nullptr;;
		if ( !pGetBestInterfaceEx || !pGetBestRoute2 )
			return false;

		NET_IFINDEX dwBestIfIndex;
		DWORD r = (*pGetBestInterfaceEx)( (sockaddr *)&sockaddrDest, &dwBestIfIndex );
		if ( r != NO_ERROR )
		{
			AssertMsg2( false, "GetBestInterfaceEx failed with result %d for address '%s'", r, CUtlNetAdrRender( addr ).String() );
			return false;
		}

		MIB_IPFORWARD_ROW2 bestRoute;
		SOCKADDR_INET bestSourceAddress;
		r = (*pGetBestRoute2)(
			nullptr, // InterfaceLuid
			dwBestIfIndex, // InterfaceIndex
			nullptr, // SourceAddress
			(SOCKADDR_INET *)&sockaddrDest, // DestinationAddress
			0, // AddressSortOptions
			&bestRoute, // BestRoute
			&bestSourceAddress // BestSourceAddress
		);
		if ( r != NO_ERROR )
		{
			AssertMsg2( false, "GetBestRoute2 failed with result %d for address '%s'", r, CUtlNetAdrRender( addr ).String() );
			return false;
		}
		if ( bestRoute.Protocol == MIB_IPPROTO_LOCAL )
			return true;
		netadr_t nextHop;
		if ( !nextHop.SetFromSockadr( &bestRoute.NextHop ) )
		{
			AssertMsg( false, "GetBestRoute2 returned invalid next hop address" );
			return false;
		}

		nextHop.SetPort( 0 );

		// https://docs.microsoft.com/en-us/windows/win32/api/netioapi/ns-netioapi-mib_ipforward_row2:
		//   For a remote route, the IP address of the next system or gateway en route.
		//   If the route is to a local loopback address or an IP address on the local
		//   link, the next hop is unspecified (all zeros). For a local loopback route,
		//   this member should be an IPv4 address of 0.0.0.0 for an IPv4 route entry
		//   or an IPv6 address address of 0::0 for an IPv6 route entry.
		if ( !nextHop.HasIP() )
			return true;
		if ( nextHop == addr )
			return true;

		// If final destination is on the same IPv6/56 prefix, then assume
		// it's a local route.  This is an arbitrary prefix size to use,
		// but it's a compromise.  We think that /64 probably has too
		// many false negatives, but /48 has have too many false positives.
		if ( addr.GetType() == k_EIPTypeV6 )
		{
			if ( nextHop.GetType() == k_EIPTypeV6 )
			{
				if ( memcmp( addr.GetIPV6Bytes(), nextHop.GetIPV6Bytes(), 7 ) == 0 )
					return true;
			}
			netadr_t netdrBestSource;
			if ( netdrBestSource.SetFromSockadr( &bestSourceAddress ) && netdrBestSource.GetType() == k_EIPTypeV6 )
			{
				if ( memcmp( addr.GetIPV6Bytes(), netdrBestSource.GetIPV6Bytes(), 7 ) == 0 )
					return true;
			}
		}

	#else
		// FIXME - Writeme
	#endif

	// Nope
	return false;
}