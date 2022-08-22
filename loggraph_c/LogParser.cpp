#include "LogParser.h"
#include "Node.h"

#include <lexer/Exception.h>

namespace loggraph
{


//////////////////////////////////////////////////////////////////////////
// class LogTokenizer
//////////////////////////////////////////////////////////////////////////

LogTokenizer::LogTokenizer(const char* str)
	: lexer::Tokenizer<LogToken::Type>(str, str + strlen(str), "\"", '\\')
	, m_skip_eol(true)
{
}

void LogTokenizer::SetSkipEol(bool skip_eol)
{
	m_skip_eol = skip_eol;
}

lexer::Tokenizer<LogToken::Type>::Token LogTokenizer::EmitToken()
{
    while (!Eof())
	{
        size_t start_line   = Line();
        size_t start_column = Column();
        const char* c = CurPos();
        switch (*c)
		{
            case '/':
                Advance();
                if (CurChar() == '/')
				{
                    Advance();
                    if (CurChar() == '/') {
                        Advance();
                        return Token(LogToken::Comment, c, c+3, Offset(c), start_line, start_column);
                    }
                    DiscardUntil("\n\r");
                }
                break;
            case '$':
            {
                LogToken::Type type;
                if (strncmp(c, "$begin", 6) == 0) {
                    type = LogToken::Begin;
                } else if (strncmp(c, "$end", 4) == 0) {
                    type = LogToken::End;
                }
                const char* e = ReadUntil(Whitespace());
                if (e == nullptr) {
                    throw lexer::ParserException(start_line, start_column, "Unexpected character: " + std::string(c, 1));
                }
                return Token(type, c, e, Offset(c), start_line, start_column);
            }
            case '"': { // quoted string
                Advance();
                c = CurPos();
                const char* e = ReadQuotedString('"', "\n}");
                return Token(LogToken::String, c, e, Offset(c), start_line, start_column);
            }
            case '\n':
                if (!m_skip_eol) {
                    Advance();
                    return Token(LogToken::Eol, c, c+1, Offset(c), start_line, start_column);
                }
//                SwitchFallthrough();
            case '\r':
            case ' ':
            case '\t':
                DiscardWhile(Whitespace());
                break;
            default: { // whitespace, integer, decimal or word
                const char* e = ReadInteger(NumberDelim());
				if (e != nullptr) {
					return Token(LogToken::Integer, c, e, Offset(c), start_line, start_column);
				}

                e = ReadDecimal(NumberDelim());
				if (e != nullptr) {
					return Token(LogToken::Decimal, c, e, Offset(c), start_line, start_column);
				}

                e = ReadUntil(Whitespace());
				if (e == nullptr) {
					throw lexer::ParserException(start_line, start_column, "Unexpected character: " + std::string(c, 1));
				}
                return Token(LogToken::String, c, e, Offset(c), start_line, start_column);
            }
        }
    }
    return Token(LogToken::Eof, nullptr, nullptr, Length(), Line(), Column());
}

const std::string& LogTokenizer::NumberDelim()
{
	static const std::string number_delim(Whitespace() + ")");
	return number_delim;
}

//////////////////////////////////////////////////////////////////////////
// class LogParser
//////////////////////////////////////////////////////////////////////////

LogParser::LogParser(const char* str)
	: m_tokenizer(LogTokenizer(str))
{
}

void LogParser::Parse()
{
    Token token = m_tokenizer.PeekToken();
    while (token.GetType() != LogToken::Eof)
    {
        Expect(LogToken::Begin, token);
        ParseNode();
        token = m_tokenizer.PeekToken();
    }
}

void LogParser::ParseNode()
{
	Token token = m_tokenizer.NextToken();
	if (token.GetType() == LogToken::Eof) {
		return;
	}

    size_t start_line = token.Line();
	while (token.GetType() != LogToken::Eof)
	{
		switch (token.GetType())
		{
		case LogToken::Begin:
            BeginNode(start_line);

            token = m_tokenizer.NextToken();
            Expect(LogToken::String, token);
            m_curr->name = token.Data();

			break;
		case LogToken::End:
			m_tokenizer.NextToken();
			EndNode(start_line, token.Line() - start_line);
			return;
        case LogToken::Integer:
            m_curr->items.push_back(token.ToInteger<int>());
            token = m_tokenizer.NextToken();
            break;
		default:
			Expect(LogToken::Comment | LogToken::String, token);
		}

		token = m_tokenizer.PeekToken();
	}
}

void LogParser::BeginNode(size_t line)
{
    m_curr = std::make_shared<Node>();
    m_nodes.push_back(m_curr);
}

void LogParser::EndNode(size_t start_line, size_t line_count)
{
    m_curr = nullptr;
}

std::map<LogToken::Type, std::string> 
LogParser::TokenNames() const
{
    using namespace LogToken;

    std::map<LogToken::Type, std::string> names;
    names[Integer] = "integer";
    names[Decimal] = "decimal";
    names[String]  = "string";
    names[Begin]   = "begin";
    names[End]     = "end";
    names[Comment] = "comment";
    names[Eof]     = "end of file";
    return names;
}

}