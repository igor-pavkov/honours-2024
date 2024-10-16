void addReplyHumanLongDouble(client *c, long double d) {
    robj *o = createStringObjectFromLongDouble(d,1);
    addReplyBulk(c,o);
    decrRefCount(o);
}