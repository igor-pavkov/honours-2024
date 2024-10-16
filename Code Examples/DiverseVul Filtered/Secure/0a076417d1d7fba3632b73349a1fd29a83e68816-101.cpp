Document literal(T&& value) {
    return Document{{"$const", Value(std::forward<T>(value))}};
}