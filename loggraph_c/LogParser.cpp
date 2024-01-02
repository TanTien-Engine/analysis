#include "LogParser.h"
#include "ProtoParser.h"
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

LogParser::LogParser(const char* data, const char* proto)
	: m_tokenizer(LogTokenizer(data))
{
    if (proto) 
    {
        ProtoParser pp(proto);
        pp.Parse();
        m_messages = pp.GetMessages();
        m_label_binds = pp.GetLabelBinds();
    }
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
	Token token = m_tokenizer.PeekToken();
	if (token.GetType() == LogToken::Eof) {
		return;
	}

    size_t start_line = token.Line();
	while (token.GetType() != LogToken::Eof)
	{
		switch (token.GetType())
		{
		case LogToken::Begin:
        {
            m_tokenizer.NextToken();

            token = m_tokenizer.NextToken();
            Expect(LogToken::String, token);
            std::string type = token.Data();

            token = m_tokenizer.NextToken();
            Expect(LogToken::String, token);
            std::string name = token.Data();

            auto node = std::make_shared<Node>(type, name);
            m_curr_nodes.push_back(node);
        }
			break;
		case LogToken::End:
        {
            m_tokenizer.NextToken();

            assert(!m_curr_nodes.empty());
            auto node = m_curr_nodes.back();
            m_curr_nodes.pop_back();

            if (m_curr_nodes.empty()) {
                m_nodes.push_back(node);
                return;
            } else {
                m_curr_nodes.back()->AddChild(node);
            }
        }
			break;
        case LogToken::Integer:
        {
            assert(!m_curr_nodes.empty());
            m_curr_nodes.back()->AddData(token.ToInteger<int>());
            token = m_tokenizer.NextToken();
        }
            break;
        case LogToken::Decimal:
        {
            assert(!m_curr_nodes.empty());
            m_curr_nodes.back()->AddData(token.ToFloat<double>());
            token = m_tokenizer.NextToken();
        }
            break;
        case LogToken::String:
        {
            assert(!m_curr_nodes.empty());

            auto str = token.Data();
            auto itr = m_label_binds.find(str);
            if (itr != m_label_binds.end()) 
            {
                token = m_tokenizer.NextToken(); // skip label
                m_curr_nodes.back()->AddData(ParseMessage(*itr->second, token));
            } 
            else 
            {
                m_curr_nodes.back()->AddData(str);
                token = m_tokenizer.NextToken();
            }
        }
            break;
		default:
        {
            Expect(LogToken::Comment, token);
            token = m_tokenizer.NextToken();
        }
		}

		token = m_tokenizer.PeekToken();
	}
    assert(m_curr_nodes.empty());
}

Variant LogParser::ParseMessage(const Message& msg, Token& token)
{
    auto& src = msg.items;
    VarGroup dst;
    dst.name = msg.name;
    for (size_t i = 0, n = src.size(); i < n; ++i)
    {
        std::string name = src[i].name;
        Variant var;

        if (src[i].repeat)
        {
            if (src[i].type == VarType::Integer)
            {
                while (m_tokenizer.PeekToken().GetType() == (int)VarType::Integer && src[i].type == VarType::Integer)
                {
                    token = m_tokenizer.NextToken();
                    var = token.ToInteger<int>();
                    dst.children.push_back({ name, var });
                }
            }
            else if (src[i].type == VarType::Double)
            {
                while (m_tokenizer.PeekToken().GetType() == (int)VarType::Double && src[i].type == VarType::Double)
                {
                    token = m_tokenizer.NextToken();
                    var = token.ToFloat<double>();
                    dst.children.push_back({ name, var });
                }
            }
        }
        else
        {
            switch (src[i].type)
            {
            case VarType::Integer:
                token = m_tokenizer.NextToken();
                var = token.ToInteger<int>();
                break;
            case VarType::Double:
                token = m_tokenizer.NextToken();
                var = token.ToFloat<double>();
                break;
            case VarType::String:
                token = m_tokenizer.NextToken();
                var = token.Data();
                break;
            case VarType::Group:
                assert(src[i].base);
                var = ParseMessage(*src[i].base, token);
                break;
            default:
                assert(0);
            }
            dst.children.push_back({ name, var });
        }
    }

    return dst;
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