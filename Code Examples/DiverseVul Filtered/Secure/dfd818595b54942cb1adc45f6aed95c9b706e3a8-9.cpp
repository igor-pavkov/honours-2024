AnyP::Uri::Uri(AnyP::UriScheme const &aScheme) :
    scheme_(aScheme),
    hostIsNumeric_(false),
    port_(0)
{
    *host_=0;
}