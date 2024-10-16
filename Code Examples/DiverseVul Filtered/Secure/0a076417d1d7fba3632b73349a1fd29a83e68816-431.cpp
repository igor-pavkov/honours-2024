    void assertContents(const intrusive_ptr<Testable>& expr, const BSONArray& expectedContents) {
        ASSERT_BSONOBJ_EQ(constify(BSON("$testable" << expectedContents)), expressionToBson(expr));
    }