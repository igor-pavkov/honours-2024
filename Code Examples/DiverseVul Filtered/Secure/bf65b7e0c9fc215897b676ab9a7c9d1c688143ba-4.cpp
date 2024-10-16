static int hashmap_complete_move(Hashmap **s, Hashmap **other) {
        assert(s);
        assert(other);

        if (!*other)
                return 0;

        if (*s)
                return hashmap_move(*s, *other);
        else
                *s = TAKE_PTR(*other);

        return 0;
}