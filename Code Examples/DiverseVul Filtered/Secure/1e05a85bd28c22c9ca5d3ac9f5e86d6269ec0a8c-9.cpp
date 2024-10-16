Client::setVirginReply(HttpReply *rep)
{
    debugs(11,5, HERE << this << " setting virgin reply to " << rep);
    assert(!theVirginReply);
    assert(rep);
    theVirginReply = rep;
    HTTPMSGLOCK(theVirginReply);
    return theVirginReply;
}