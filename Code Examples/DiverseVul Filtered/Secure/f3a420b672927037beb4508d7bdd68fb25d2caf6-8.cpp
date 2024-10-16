lexer_next_token (parser_context_t *context_p) /**< context */
{
  size_t length;

#if JERRY_ESNEXT && JERRY_FUNCTION_TO_STRING
  /* Needed by arrow functions with expression body */
  context_p->function_end_p = context_p->source_p;
#endif /* JERRY_ESNEXT && JERRY_FUNCTION_TO_STRING */

  lexer_skip_spaces (context_p);

  context_p->token.keyword_type = LEXER_EOS;
  context_p->token.line = context_p->line;
  context_p->token.column = context_p->column;

  length = (size_t) (context_p->source_end_p - context_p->source_p);
  if (length == 0)
  {
    context_p->token.type = LEXER_EOS;
    return;
  }

  if (lexer_parse_identifier (context_p, LEXER_PARSE_CHECK_KEYWORDS))
  {
    return;
  }

  if (context_p->source_p[0] >= LIT_CHAR_0 && context_p->source_p[0] <= LIT_CHAR_9)
  {
    lexer_parse_number (context_p);
    return;
  }

  switch (context_p->source_p[0])
  {
    LEXER_TYPE_A_TOKEN (LIT_CHAR_LEFT_BRACE, LEXER_LEFT_BRACE);
    LEXER_TYPE_A_TOKEN (LIT_CHAR_LEFT_PAREN, LEXER_LEFT_PAREN);
    LEXER_TYPE_A_TOKEN (LIT_CHAR_LEFT_SQUARE, LEXER_LEFT_SQUARE);
    LEXER_TYPE_A_TOKEN (LIT_CHAR_RIGHT_BRACE, LEXER_RIGHT_BRACE);
    LEXER_TYPE_A_TOKEN (LIT_CHAR_RIGHT_PAREN, LEXER_RIGHT_PAREN);
    LEXER_TYPE_A_TOKEN (LIT_CHAR_RIGHT_SQUARE, LEXER_RIGHT_SQUARE);
    LEXER_TYPE_A_TOKEN (LIT_CHAR_SEMICOLON, LEXER_SEMICOLON);
    LEXER_TYPE_A_TOKEN (LIT_CHAR_COMMA, LEXER_COMMA);
#if JERRY_ESNEXT
    LEXER_TYPE_A_TOKEN (LIT_CHAR_HASHMARK, LEXER_HASHMARK);
#endif /* JERRY_ESNEXT */

    case (uint8_t) LIT_CHAR_DOT:
    {
      if (length >= 2 && (context_p->source_p[1] >= LIT_CHAR_0 && context_p->source_p[1] <= LIT_CHAR_9))
      {
        lexer_parse_number (context_p);
        return;
      }

#if JERRY_ESNEXT
      if (length >= 3 && context_p->source_p[1] == LIT_CHAR_DOT && context_p->source_p[2] == LIT_CHAR_DOT)
      {
        context_p->token.type = LEXER_THREE_DOTS;
        length = 3;
        break;
      }
#endif /* JERRY_ESNEXT */

      context_p->token.type = LEXER_DOT;
      length = 1;
      break;
    }

    case (uint8_t) LIT_CHAR_LESS_THAN:
    {
      if (length >= 2)
      {
        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_EQUALS)
        {
          context_p->token.type = LEXER_LESS_EQUAL;
          length = 2;
          break;
        }

        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_LESS_THAN)
        {
          if (length >= 3 && context_p->source_p[2] == (uint8_t) LIT_CHAR_EQUALS)
          {
            context_p->token.type = LEXER_ASSIGN_LEFT_SHIFT;
            length = 3;
            break;
          }

          context_p->token.type = LEXER_LEFT_SHIFT;
          length = 2;
          break;
        }
      }

      context_p->token.type = LEXER_LESS;
      length = 1;
      break;
    }

    case (uint8_t) LIT_CHAR_GREATER_THAN:
    {
      if (length >= 2)
      {
        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_EQUALS)
        {
          context_p->token.type = LEXER_GREATER_EQUAL;
          length = 2;
          break;
        }

        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_GREATER_THAN)
        {
          if (length >= 3)
          {
            if (context_p->source_p[2] == (uint8_t) LIT_CHAR_EQUALS)
            {
              context_p->token.type = LEXER_ASSIGN_RIGHT_SHIFT;
              length = 3;
              break;
            }

            if (context_p->source_p[2] == (uint8_t) LIT_CHAR_GREATER_THAN)
            {
              if (length >= 4 && context_p->source_p[3] == (uint8_t) LIT_CHAR_EQUALS)
              {
                context_p->token.type = LEXER_ASSIGN_UNS_RIGHT_SHIFT;
                length = 4;
                break;
              }

              context_p->token.type = LEXER_UNS_RIGHT_SHIFT;
              length = 3;
              break;
            }
          }

          context_p->token.type = LEXER_RIGHT_SHIFT;
          length = 2;
          break;
        }
      }

      context_p->token.type = LEXER_GREATER;
      length = 1;
      break;
    }

    case (uint8_t) LIT_CHAR_EQUALS:
    {
      if (length >= 2)
      {
        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_EQUALS)
        {
          if (length >= 3 && context_p->source_p[2] == (uint8_t) LIT_CHAR_EQUALS)
          {
            context_p->token.type = LEXER_STRICT_EQUAL;
            length = 3;
            break;
          }

          context_p->token.type = LEXER_EQUAL;
          length = 2;
          break;
        }

#if JERRY_ESNEXT
        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_GREATER_THAN)
        {
          context_p->token.type = LEXER_ARROW;
          length = 2;
          break;
        }
#endif /* JERRY_ESNEXT */
      }

      context_p->token.type = LEXER_ASSIGN;
      length = 1;
      break;
    }

    case (uint8_t) LIT_CHAR_EXCLAMATION:
    {
      if (length >= 2 && context_p->source_p[1] == (uint8_t) LIT_CHAR_EQUALS)
      {
        if (length >= 3 && context_p->source_p[2] == (uint8_t) LIT_CHAR_EQUALS)
        {
          context_p->token.type = LEXER_STRICT_NOT_EQUAL;
          length = 3;
          break;
        }

        context_p->token.type = LEXER_NOT_EQUAL;
        length = 2;
        break;
      }

      context_p->token.type = LEXER_LOGICAL_NOT;
      length = 1;
      break;
    }

      LEXER_TYPE_C_TOKEN (LIT_CHAR_PLUS, LEXER_ADD, LIT_CHAR_EQUALS, LEXER_ASSIGN_ADD, LIT_CHAR_PLUS, LEXER_INCREASE)
      LEXER_TYPE_C_TOKEN (LIT_CHAR_MINUS,
                          LEXER_SUBTRACT,
                          LIT_CHAR_EQUALS,
                          LEXER_ASSIGN_SUBTRACT,
                          LIT_CHAR_MINUS,
                          LEXER_DECREASE)

    case (uint8_t) LIT_CHAR_ASTERISK:
    {
      if (length >= 2)
      {
        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_EQUALS)
        {
          context_p->token.type = LEXER_ASSIGN_MULTIPLY;
          length = 2;
          break;
        }

#if JERRY_ESNEXT
        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_ASTERISK)
        {
          if (length >= 3 && context_p->source_p[2] == (uint8_t) LIT_CHAR_EQUALS)
          {
            context_p->token.type = LEXER_ASSIGN_EXPONENTIATION;
            length = 3;
            break;
          }

          context_p->token.type = LEXER_EXPONENTIATION;
          length = 2;
          break;
        }
#endif /* JERRY_ESNEXT */
      }

      context_p->token.type = LEXER_MULTIPLY;
      length = 1;
      break;
    }

      LEXER_TYPE_B_TOKEN (LIT_CHAR_SLASH, LEXER_DIVIDE, LIT_CHAR_EQUALS, LEXER_ASSIGN_DIVIDE)
      LEXER_TYPE_B_TOKEN (LIT_CHAR_PERCENT, LEXER_MODULO, LIT_CHAR_EQUALS, LEXER_ASSIGN_MODULO)

      LEXER_TYPE_D_TOKEN (LIT_CHAR_AMPERSAND,
                          LEXER_BIT_AND,
                          LIT_CHAR_EQUALS,
                          LEXER_ASSIGN_BIT_AND,
                          LIT_CHAR_AMPERSAND,
                          LEXER_LOGICAL_AND,
                          LIT_CHAR_EQUALS,
                          LEXER_ASSIGN_LOGICAL_AND)
      LEXER_TYPE_D_TOKEN (LIT_CHAR_VLINE,
                          LEXER_BIT_OR,
                          LIT_CHAR_EQUALS,
                          LEXER_ASSIGN_BIT_OR,
                          LIT_CHAR_VLINE,
                          LEXER_LOGICAL_OR,
                          LIT_CHAR_EQUALS,
                          LEXER_ASSIGN_LOGICAL_OR)

      LEXER_TYPE_B_TOKEN (LIT_CHAR_CIRCUMFLEX, LEXER_BIT_XOR, LIT_CHAR_EQUALS, LEXER_ASSIGN_BIT_XOR)

      LEXER_TYPE_A_TOKEN (LIT_CHAR_TILDE, LEXER_BIT_NOT);
    case (uint8_t) (LIT_CHAR_QUESTION):
    {
#if JERRY_ESNEXT
      if (length >= 2)
      {
        if (context_p->source_p[1] == (uint8_t) LIT_CHAR_QUESTION)
        {
          if (length >= 3 && context_p->source_p[2] == (uint8_t) LIT_CHAR_EQUALS)
          {
            context_p->token.type = LEXER_ASSIGN_NULLISH_COALESCING;
            length = 3;
            break;
          }
          context_p->token.type = LEXER_NULLISH_COALESCING;
          length = 2;
          break;
        }
      }
#endif /* JERRY_ESNEXT */
      context_p->token.type = LEXER_QUESTION_MARK;
      length = 1;
      break;
    }

      LEXER_TYPE_A_TOKEN (LIT_CHAR_COLON, LEXER_COLON);

    case LIT_CHAR_SINGLE_QUOTE:
    case LIT_CHAR_DOUBLE_QUOTE:
#if JERRY_ESNEXT
    case LIT_CHAR_GRAVE_ACCENT:
#endif /* JERRY_ESNEXT */
    {
      lexer_parse_string (context_p, LEXER_STRING_NO_OPTS);
      return;
    }

    default:
    {
      parser_raise_error (context_p, PARSER_ERR_INVALID_CHARACTER);
    }
  }

  context_p->source_p += length;
  PARSER_PLUS_EQUAL_LC (context_p->column, length);
} /* lexer_next_token */