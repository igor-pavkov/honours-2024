static int forgive(int n,int& err)
{
    err  = WSAGetLastError() ;
    if ( !n && !err ) return FINISH ;
#ifndef WIN32
    if ( n == 0 ) return FINISH ; // server hungup
#endif
    bool bForgive = err == WSAEWOULDBLOCK || err == WSAENOTCONN ;
    bool bError   = n == SOCKET_ERROR ;
    if ( bError && bForgive ) return 0 ;
    return n ;
}