#pragma once

#include "Variant.h"

#include <lexer/Tokenizer.h>
#include <lexer/Parser.h>

#include <string>
#include <vector>
#include <map>

namespace loggraph
{

struct Message
{
	struct Item
	{
		VarType type;
		std::shared_ptr<Message> base = nullptr;

		std::string name;

		bool repeat = false;
	};

	std::string name;
	std::vector<Item> items;

}; // Message

namespace ProtoToken
{
	typedef unsigned int Type;
	static const Type Integer       = 1 <<  0; // integer number
	static const Type Decimal       = 1 <<  1; // decimal number
	static const Type String        = 1 <<  2; // string
	static const Type Message       = 1 <<  3; // message
	static const Type Bind          = 1 <<  4; // bind
	static const Type OBrace        = 1 <<  8; // opening brace: {
	static const Type CBrace        = 1 <<  9; // closing brace: }
	static const Type Comment       = 1 << 10; // line comment starting with ///
	static const Type Eof           = 1 << 11; // end of file
	static const Type Eol           = 1 << 12; // end of line
}

class ProtoTokenizer : public lexer::Tokenizer<ProtoToken::Type>
{
public:
	ProtoTokenizer(const char* str);

	void SetSkipEol(bool skip_eol);

protected:
	virtual Token EmitToken() override;

	static const std::string& NumberDelim();

private:
	bool m_skip_eol;

}; // ProtoTokenizer

class ProtoParser : public lexer::Parser<ProtoToken::Type>
{
public:
	ProtoParser(const char* str);

	void Parse();
	
	auto& GetMessages() const { return m_messages; }
	auto& GetLabelBinds() const { return m_label_binds; }

private:
	void ParseMessage();

	std::shared_ptr<Message> QueryMessage(const std::string& name) const;

	virtual std::map<ProtoToken::Type, std::string> TokenNames() const override;

private:
	ProtoTokenizer m_tokenizer;

	std::vector<std::shared_ptr<Message>> m_messages;
	std::map<std::string, std::shared_ptr<Message>> m_label_binds;

	typedef ProtoTokenizer::Token Token;

}; // ProtoParser

}