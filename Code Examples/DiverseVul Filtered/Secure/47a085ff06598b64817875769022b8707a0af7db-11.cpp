UrnState::setUriResFromRequest(HttpRequest *r)
{
    const auto &query = r->url.absolute();
    const auto host = r->url.host();
    // TODO: use class AnyP::Uri instead of generating a string and re-parsing
    LOCAL_ARRAY(char, local_urlres, 4096);
    snprintf(local_urlres, 4096, "http://%s/uri-res/N2L?" SQUIDSBUFPH, host, SQUIDSBUFPRINT(query));
    safe_free(urlres);
    urlres_r = HttpRequest::FromUrlXXX(local_urlres, r->masterXaction);

    if (!urlres_r) {
        debugs(52, 3, "Bad uri-res URL " << local_urlres);
        const auto err = new ErrorState(ERR_URN_RESOLVE, Http::scNotFound, r, ale);
        err->url = xstrdup(local_urlres);
        errorAppendEntry(entry, err);
        return;
    }

    urlres = xstrdup(local_urlres);
    urlres_r->header.putStr(Http::HdrType::ACCEPT, "text/plain");
}