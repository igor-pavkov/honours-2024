AnyP::Uri::hostOrIp() const
{
    static char ip[MAX_IPSTRLEN];
    if (hostIsNumeric())
        return SBuf(hostIP().toStr(ip, sizeof(ip)));
    else
        return SBuf(host());
}