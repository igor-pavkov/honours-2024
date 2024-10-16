Client::maybePurgeOthers()
{
    // only some HTTP methods should purge matching cache entries
    if (!request->method.purgesOthers())
        return;

    // and probably only if the response was successful
    if (theFinalReply->sline.status() >= 400)
        return;

    // XXX: should we use originalRequest() here?
    SBuf tmp(request->effectiveRequestUri());
    const char *reqUrl = tmp.c_str();
    debugs(88, 5, "maybe purging due to " << request->method << ' ' << tmp);
    purgeEntriesByUrl(request, reqUrl);
    purgeEntriesByHeader(request, reqUrl, theFinalReply, Http::HdrType::LOCATION);
    purgeEntriesByHeader(request, reqUrl, theFinalReply, Http::HdrType::CONTENT_LOCATION);
}