#pragma once

#include "Variant.h"

#include <lexer/Tokenizer.h>
#include <lexer/Parser.h>

namespace loggraph
{

struct Message;
class Node;

namespace LogToken
{
	typedef unsigned int Type;
	static const Type Integer       = 1 <<  0; // integer number
	static const Type Decimal       = 1 <<  1; // decimal number
	static const Type String        = 1 <<  2; // string
	static const Type Begin         = 1 <<  3; // opening parenthesis: (
	static const Type End           = 1 <<  4; // closing parenthesis: )
	static const Type OBrace		= 1 <<  5; // opening brace: {
	static const Type CBrace		= 1 <<  6; // closing brace: }
	static const Type OBracket		= 1 <<	7; // opening bracket: [
	static const Type CBracket		= 1 <<	8; // closing Bracket: ]
	static const Type Comment       = 1 <<  9; // line comment starting with ///
	static const Type Eof           = 1 << 10; // end of file
	static const Type Eol           = 1 << 11; // end of line
}

class LogTokenizer : public lexer::Tokenizer<LogToken::Type>
{
public:
	LogTokenizer(const char* str);

	void SetSkipEol(bool skip_eol);

protected:
	virtual Token EmitToken() override;

	static const std::string& NumberDelim();

private:
	bool m_skip_eol;

}; // LogTokenizer

class LogParser : public lexer::Parser<LogToken::Type>
{
public:
	LogParser(const char* data, const char* proto);

	void Parse();
	
	auto& GetNodes() const { return m_nodes; }

private:
	void ParseNode();

	Variant ParseMessage(const Message& msg, LogTokenizer::Token& token);

	virtual std::map<LogToken::Type, std::string> TokenNames() const override;

private:
	LogTokenizer m_tokenizer;

	// proto
	std::vector<std::shared_ptr<Message>> m_messages;
	std::map<std::string, std::shared_ptr<Message>> m_label_binds;

	std::vector<std::shared_ptr<Node>> m_nodes;
	std::vector<std::shared_ptr<Node>> m_curr_nodes;

	typedef LogTokenizer::Token Token;

}; // LogParser

}