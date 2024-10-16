        bool empty() const {
          return !prefix &&
            (tags.size() == 0);
        }