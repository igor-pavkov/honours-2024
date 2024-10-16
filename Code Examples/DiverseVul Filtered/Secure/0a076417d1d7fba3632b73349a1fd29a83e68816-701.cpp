    static BSONArray toBsonArray(const intrusive_ptr<Expression>& expression) {
        BSONArrayBuilder bab;
        bab << expression->serialize(false);
        return bab.arr();
    }