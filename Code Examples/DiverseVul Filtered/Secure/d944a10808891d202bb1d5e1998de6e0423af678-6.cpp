void ReallyReportBadUDPPacket( const char *pszFrom, const char *pszMsgType, const char *pszFmt, ... )
{
	char buf[ 2048 ];
	va_list ap;
	va_start( ap, pszFmt );
	V_vsprintf_safe( buf, pszFmt, ap );
	va_end( ap );
	V_StripTrailingWhitespaceASCII( buf );

	if ( !pszMsgType || !pszMsgType[0] )
		pszMsgType = "message";

	SpewMsg( "[%s] Ignored bad %s.  %s\n", pszMsgType, pszFrom, buf );
}