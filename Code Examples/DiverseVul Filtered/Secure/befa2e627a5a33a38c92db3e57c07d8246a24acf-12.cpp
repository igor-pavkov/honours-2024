    uint64_t translateMask( const std::string mask )
    {
      if( mask == "" )
        return 0xffffffffffffffffULL;

      std::vector<std::string>           topics;
      std::vector<std::string>::iterator it;
      XrdCl::Utils::splitString( topics, mask, "|" );

      uint64_t resultMask = 0;
      std::map<std::string, uint64_t>::iterator maskIt;
      for( it = topics.begin(); it != topics.end(); ++it )
      {
        //----------------------------------------------------------------------
        // Check for resetting pseudo topics
        //----------------------------------------------------------------------
        if( *it == "All" )
        {
          resultMask = 0xffffffffffffffffULL;
          continue;
        }

        if( *it == "None" )
        {
          resultMask = 0ULL;
          continue;
        }

        //----------------------------------------------------------------------
        // Check whether given topic should be disabled or enabled
        //----------------------------------------------------------------------
        std::string topic = *it;
        bool disable      = false;
        if( !topic.empty() && topic[0] == '^' )
        {
          disable = true;
          topic   = topic.substr( 1, topic.length()-1 );
        }

        maskIt = masks.find( topic );
        if( maskIt == masks.end() )
          continue;

        if( disable )
          resultMask &= (0xffffffffffffffffULL ^ maskIt->second);
        else
          resultMask |= maskIt->second;
      }

      return resultMask;
    }