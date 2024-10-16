    TEST(BSONValidateFast, NonTopLevelId) {
        BufBuilder bb;
        BSONObjBuilder ob(bb);
        ob.append("not_id1", BSON("_id" << "not the real _id"));
        appendInvalidStringElement("not_id2", &bb);
        const BSONObj x = ob.done();
        const Status status = validateBSON(x.objdata(), x.objsize());
        ASSERT_NOT_OK(status);
        ASSERT_EQUALS(status.reason(), "not null terminated string in object with unknown _id");
    }