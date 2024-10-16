        query_string(std::string params, bool url = true):
          url_(std::move(params))
        {
            if (url_.empty())
                return;

            key_value_pairs_.resize(MAX_KEY_VALUE_PAIRS_COUNT);

            int count = qs_parse(&url_[0], &key_value_pairs_[0], MAX_KEY_VALUE_PAIRS_COUNT, url);
            key_value_pairs_.resize(count);
        }