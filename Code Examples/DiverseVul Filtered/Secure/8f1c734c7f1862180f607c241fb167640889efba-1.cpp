    TEST(BSONValidateFast, ErrorIsInId) {
        BufBuilder bb;
        BSONObjBuilder ob(bb);
        appendInvalidStringElement("_id", &bb);
        const BSONObj x = ob.done();
        const Status status = validateBSON(x.objdata(), x.objsize());
        ASSERT_NOT_OK(status);
        ASSERT_EQUALS(status.reason(), "not null terminated string in object with unknown _id");
    }