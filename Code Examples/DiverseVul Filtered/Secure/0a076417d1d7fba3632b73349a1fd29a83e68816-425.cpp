Value valueFromBson(BSONObj obj) {
    BSONElement element = obj.firstElement();
    return Value(element);
}