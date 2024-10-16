int clientHasPendingReplies(client *c) {
    return c->bufpos || listLength(c->reply);
}