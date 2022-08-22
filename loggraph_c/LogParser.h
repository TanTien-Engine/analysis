#pragma once

#include <lexer/Tokenizer.h>
#include <lexer/Parser.h>

namespace loggraph
{

class Node;

namespace LogToken
{
	typedef unsigned int Type;
	static const Type Integer       = 1 <<  0; // integer number
	static const Type Decimal       = 1 <<  1; // decimal number
	static const Type String        = 1 <<  2; // string
	static const Type Begin         = 1 <<  3; // opening parenthesis: (
	static const Type End           = 1 <<  4; // closing parenthesis: )
	static const Type Comment       = 1 <<  5; // line comment starting with ///
	static const Type Eof           = 1 <<  6; // end of file
	static const Type Eol           = 1 <<  7; // end of line
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
	LogParser(const char* str);

	void Parse();
	
	auto& GetNodes() const { return m_nodes; }

private:
	void ParseNode();

	void BeginNode(size_t line);
	void EndNode(size_t start_line, size_t line_count);

	virtual std::map<LogToken::Type, std::string> TokenNames() const override;

private:
	LogTokenizer m_tokenizer;

	std::vector<std::shared_ptr<Node>> m_nodes;
	std::shared_ptr<Node> m_curr = nullptr;

	typedef LogTokenizer::Token Token;

}; // LogParser

}