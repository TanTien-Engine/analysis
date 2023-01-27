#include "ProtoParser.h"

#include <lexer/Exception.h>

#include <assert.h>

namespace loggraph
{


//////////////////////////////////////////////////////////////////////////
// class ProtoTokenizer
//////////////////////////////////////////////////////////////////////////

ProtoTokenizer::ProtoTokenizer(const char* str)
	: lexer::Tokenizer<ProtoToken::Type>(str, str + strlen(str), "\"", '\\')
	, m_skip_eol(true)
{
}

void ProtoTokenizer::SetSkipEol(bool skip_eol)
{
	m_skip_eol = skip_eol;
}

lexer::Tokenizer<ProtoToken::Type>::Token ProtoTokenizer::EmitToken()
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
                        return Token(ProtoToken::Comment, c, c+3, Offset(c), start_line, start_column);
                    }
                    DiscardUntil("\n\r");
                }
                break;
            case '{':
                Advance();
                return Token(ProtoToken::OBrace, c, c + 1, Offset(c), start_line, start_column);
            case '}':
                Advance();
                return Token(ProtoToken::CBrace, c, c + 1, Offset(c), start_line, start_column);
            case '\n':
                if (!m_skip_eol) {
                    Advance();
                    return Token(ProtoToken::Eol, c, c+1, Offset(c), start_line, start_column);
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
					return Token(ProtoToken::Integer, c, e, Offset(c), start_line, start_column);
				}

                e = ReadDecimal(NumberDelim());
				if (e != nullptr) {
					return Token(ProtoToken::Decimal, c, e, Offset(c), start_line, start_column);
				}

                e = ReadUntil(Whitespace());
				if (e == nullptr) {
					throw lexer::ParserException(start_line, start_column, "Unexpected character: " + std::string(c, 1));
				}

                ProtoToken::Type str_type;
                if (strncmp(c, "message", e - c) == 0) {
                    str_type = ProtoToken::Message;
                } else {
                    str_type = ProtoToken::String;
                }
                return Token(str_type, c, e, Offset(c), start_line, start_column);
            }
        }
    }
    return Token(ProtoToken::Eof, nullptr, nullptr, Length(), Line(), Column());
}

const std::string& ProtoTokenizer::NumberDelim()
{
	static const std::string number_delim(Whitespace() + ")");
	return number_delim;
}


//////////////////////////////////////////////////////////////////////////
// class ProtoParser
//////////////////////////////////////////////////////////////////////////

ProtoParser::ProtoParser(const char* str)
	: m_tokenizer(ProtoTokenizer(str))
{
}

void ProtoParser::Parse()
{
    Token token = m_tokenizer.PeekToken();
    while (token.GetType() != ProtoToken::Eof)
    {
        Expect(ProtoToken::Message, token);
        ParseMessage();
        token = m_tokenizer.PeekToken();
    }
}

void ProtoParser::ParseMessage()
{
	Token token = m_tokenizer.PeekToken();
	if (token.GetType() == ProtoToken::Eof) {
		return;
	}

    size_t start_line = token.Line();
	while (token.GetType() != ProtoToken::Eof)
	{
		switch (token.GetType())
		{
        case ProtoToken::Message:
        {
            m_tokenizer.NextToken();

            token = m_tokenizer.NextToken();
            Expect(ProtoToken::String, token);
            std::string name = token.Data();

            token = m_tokenizer.NextToken();
            Expect(ProtoToken::OBrace, token);

            auto msg = std::make_shared<Message>();
            msg->name = name;
            while (!Check(ProtoToken::CBrace, m_tokenizer.PeekToken()))
            {
                Message::Item item;

                token = m_tokenizer.NextToken();
                Expect(ProtoToken::String, token);
                std::string type = token.Data();
                if (type == "int") {
                    item.type = VarType::Integer;
                } else if (type == "double") {
                    item.type = VarType::Double;
                } else if (type == "string") {
                    item.type = VarType::String;
                } else {
                    item.type = VarType::Invalid;
                    for (auto& msg : m_messages) {
                        if (msg->name == type) {
                            item.type = VarType::Struct;
                            item.base = msg;
                        }
                    }
                }

                token = m_tokenizer.NextToken();
                Expect(ProtoToken::String, token);
                item.name = token.Data();

                msg->items.push_back(item);
            }

            m_messages.push_back(msg);

            token = m_tokenizer.NextToken();
        }
            break;
		default:
        {
            Expect(ProtoToken::Comment, token);
            token = m_tokenizer.NextToken();
        }
		}

		token = m_tokenizer.PeekToken();
	}
}

std::map<ProtoToken::Type, std::string> 
ProtoParser::TokenNames() const
{
    using namespace ProtoToken;

    std::map<ProtoToken::Type, std::string> names;
    names[Integer] = "integer";
    names[Decimal] = "decimal";
    names[String]  = "string";
    names[OBrace]  = "o_brace";
    names[CBrace]  = "c_brace";
    names[Comment] = "comment";
    names[Eof]     = "end of file";
    return names;
}

}