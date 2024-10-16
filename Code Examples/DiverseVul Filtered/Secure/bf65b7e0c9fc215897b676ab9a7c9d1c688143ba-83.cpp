static int set_complete_move(Set **s, Set **other) {
        assert(s);
        assert(other);

        if (!other)
                return 0;

        if (*s)
                return set_move(*s, *other);
        else
                *s = TAKE_PTR(*other);

        return 0;
}