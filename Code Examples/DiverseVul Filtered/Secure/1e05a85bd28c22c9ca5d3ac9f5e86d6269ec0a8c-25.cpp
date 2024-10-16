Client::blockCaching()
{
    if (const Acl::Tree *acl = Config.accessList.storeMiss) {
        // This relatively expensive check is not in StoreEntry::checkCachable:
        // That method lacks HttpRequest and may be called too many times.
        ACLFilledChecklist ch(acl, originalRequest(), NULL);
        ch.reply = const_cast<HttpReply*>(entry->getReply()); // ACLFilledChecklist API bug
        HTTPMSGLOCK(ch.reply);
        if (!ch.fastCheck().allowed()) { // when in doubt, block
            debugs(20, 3, "store_miss prohibits caching");
            return true;
        }
    }
    return false;
}