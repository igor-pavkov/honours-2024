    TEST(BSONValidateFast, ErrorWithId) {
        BufBuilder bb;
        BSONObjBuilder ob(bb);
        ob.append("_id", 1);
        appendInvalidStringElement("not_id", &bb);
        const BSONObj x = ob.done();
        const Status status = validateBSON(x.objdata(), x.objsize());
        ASSERT_NOT_OK(status);
        ASSERT_EQUALS(status.reason(), "not null terminated string in object with _id: 1");
    }