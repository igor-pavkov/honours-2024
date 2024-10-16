TEST_F(ExpressionNaryTest, ValidateEmptyDependencies) {
    assertDependencies(_notAssociativeNorCommutative, BSONArray());
}