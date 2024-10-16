AnyP::Uri::authority(bool requirePort) const
{
    if (authorityHttp_.isEmpty()) {

        // both formats contain Host/IP
        authorityWithPort_.append(host());
        authorityHttp_ = authorityWithPort_;

        // authorityForm_ only has :port if it is non-default
        authorityWithPort_.appendf(":%u",port());
        if (port() != getScheme().defaultPort())
            authorityHttp_ = authorityWithPort_;
    }

    return requirePort ? authorityWithPort_ : authorityHttp_;
}