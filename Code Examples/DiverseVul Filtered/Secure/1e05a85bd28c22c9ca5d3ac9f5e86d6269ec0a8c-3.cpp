Http::Stream::registerWithConn()
{
    assert(!connRegistered_);
    assert(getConn());
    connRegistered_ = true;
    getConn()->pipeline.add(Http::StreamPointer(this));
}