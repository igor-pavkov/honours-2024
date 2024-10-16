    static BSONObj toBsonObj(const intrusive_ptr<Expression>& expression) {
        return BSON("field" << expression->serialize(false));
    }