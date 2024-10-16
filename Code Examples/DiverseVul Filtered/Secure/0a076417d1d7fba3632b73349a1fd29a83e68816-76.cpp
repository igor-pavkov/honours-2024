    void run() {
        intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
        const Document spec = getSpec();
        const Value args = spec["input"];
        if (!spec["expected"].missing()) {
            FieldIterator fields(spec["expected"].getDocument());
            while (fields.more()) {
                const Document::FieldPair field(fields.next());
                const Value expected = field.second;
                const BSONObj obj = BSON(field.first << args);
                VariablesParseState vps = expCtx->variablesParseState;
                const intrusive_ptr<Expression> expr =
                    Expression::parseExpression(expCtx, obj, vps);
                const Value result = expr->evaluate(Document());
                if (ValueComparator().evaluate(result != expected)) {
                    string errMsg = str::stream()
                        << "for expression " << field.first.toString() << " with argument "
                        << args.toString() << " full tree: " << expr->serialize(false).toString()
                        << " expected: " << expected.toString()
                        << " but got: " << result.toString();
                    FAIL(errMsg);
                }
                // TODO test optimize here
            }
        }
        if (!spec["error"].missing()) {
            const vector<Value>& asserters = spec["error"].getArray();
            size_t n = asserters.size();
            for (size_t i = 0; i < n; i++) {
                const BSONObj obj = BSON(asserters[i].getString() << args);
                VariablesParseState vps = expCtx->variablesParseState;
                ASSERT_THROWS(
                    {
                        // NOTE: parse and evaluatation failures are treated the
                        // same
                        const intrusive_ptr<Expression> expr =
                            Expression::parseExpression(expCtx, obj, vps);
                        expr->evaluate(Document());
                    },
                    AssertionException);
            }
        }
    }