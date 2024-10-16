static const char *req_proxyreq_field(request_rec *r)
{
    switch (r->proxyreq) {
        case PROXYREQ_NONE:     return "PROXYREQ_NONE";
        case PROXYREQ_PROXY:    return "PROXYREQ_PROXY";
        case PROXYREQ_REVERSE:  return "PROXYREQ_REVERSE";
        case PROXYREQ_RESPONSE: return "PROXYREQ_RESPONSE";
        default: return NULL;
    }
}