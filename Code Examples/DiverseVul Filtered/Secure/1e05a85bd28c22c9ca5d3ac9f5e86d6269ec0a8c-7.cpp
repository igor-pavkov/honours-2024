Http::Stream::finished()
{
    ConnStateData *conn = getConn();

    /* we can't handle any more stream data - detach */
    clientStreamDetach(getTail(), http);

    assert(connRegistered_);
    connRegistered_ = false;
    conn->pipeline.popMe(Http::StreamPointer(this));
}