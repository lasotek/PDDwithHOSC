#pragma once
#include "StdAfx.h"
#include <stdexcept>
#include <xstring>
using namespace std;

#define FILE_NAME __FILE__
#define LINE_NO __LINE__


class eNotImplementedOperation : public exception
{
public:
	eNotImplementedOperation(const char* _S="Not implemented yet"):exception(_S) {}
	~eNotImplementedOperation() {}
};

class eWrongSourceIdentifier : public exception
{
public:
	eWrongSourceIdentifier(const char* _S="Wrong identifier of the source"):exception(_S) {}
	~eWrongSourceIdentifier() {}
};

class eWrongCondition : public exception
{
public:
	eWrongCondition(const char* _S="Wrong condition\\initialization"):exception(_S) {}
	~eWrongCondition() {}
};

class eWrongComponentType : public exception
{
public:
	eWrongComponentType(const char* _S="Wrong component"):exception(_S) {}
	~eWrongComponentType() {}
};
class eIllegalOperation : public exception
{
public:
	eIllegalOperation(const char* _S):exception(_S) {}
	~eIllegalOperation() {}
};

class eWrongName : public exception
{
public:
	eWrongName(const char* _S="Wrong component name"):exception(_S) {}
	~eWrongName() {}
};

class eWrongNodeName : public exception
{
public:
	eWrongNodeName(const char* _S=""):exception(_S) {}
	~eWrongNodeName() {}
};

class eNodeNameRedefinition : public exception
{
public:
	eNodeNameRedefinition(const char* _S=""):exception(_S) {}
	~eNodeNameRedefinition() {}
};

class eFilerProblem : public exception
{
public:
	eFilerProblem(const char* _S=""):exception(_S) {}
	~eFilerProblem() {}
};

class eWrongFormatConnection : public exception
{
public:
	eWrongFormatConnection(const char* _S="Wrong parameter format\n"
										  "<circuit_node_name> = <model_node_name> is expected"):exception(_S) {}
	~eWrongFormatConnection() {}
};

class eIllegalModelName : public exception
{
public:
	~eIllegalModelName() {}
	eIllegalModelName(const char* _S="The subcircuit the function refers to doesn't exist"):exception(_S) {}
};

class eConnectionDoesntFits : public exception
{
public:
	eConnectionDoesntFits(const char* _S="The number of connection should be equal to the number of the subcircuit ports.\n"
		"Hint: Check the proper connecton of the reference node"):exception(_S) {}
	~eConnectionDoesntFits() {}
};

class eIllegalBoundaryNode : public exception
{
public:
	eIllegalBoundaryNode(const char* _S=""):exception(_S) {}
	~eIllegalBoundaryNode() {}
};

class eMoreThanOneModelFile : public exception
{
public:
	eMoreThanOneModelFile(const char* _S=""):exception(_S) {}
	~eMoreThanOneModelFile() {}
};

class eNameDuplicated : public exception
{
public:
	eNameDuplicated(const char* _S=""):exception(_S) {}
	~eNameDuplicated() {}
};

class eUnexpectedCondition : public exception
{
public:
	eUnexpectedCondition(const char* _S=""):exception(_S) {}
	~eUnexpectedCondition() {}
};

class eVertexEntryUndifined : public exception
{
public:
	eVertexEntryUndifined(const char* _S=""):exception(_S) {}
	~eVertexEntryUndifined() {}
};

class eContextUndifined : public exception
{
public:
	eContextUndifined(const char* _S=""):exception(_S) {}
	~eContextUndifined() {}
};

class eNameDuplication : public exception
{
public:
	eNameDuplication(const char* _S=""):exception(_S) {}
	~eNameDuplication() {}
};

class eWrongNextOperationId : public exception
{
public:
	eWrongNextOperationId(const char* _S=""):exception(_S) {}
	~eWrongNextOperationId() {}
};

class eMultipleDivRemOperation : public exception
{
public:
	eMultipleDivRemOperation(const char* _S=""):exception(_S) {}
	~eMultipleDivRemOperation() {}
};

class eWrongPins : public exception
{
public:
	eWrongPins(const char* _S=""):exception(_S) {}
	~eWrongPins() {}
};

const char* ErrorMessage(const char* Comment, const char* Filename, int LineNumber);

#define VERIFYPDD(Cond) if((Cond)!=true) throw eIllegalOperation(ErrorMessage("ASSERT condition was rised",FILE_NAME,LINE_NO));

#define RISEPDD(Exception,TextMessage) throw Exception(ErrorMessage(TextMessage,FILE_NAME,LINE_NO));

#ifdef _DEBUG
#define ASSERTPDD(Cond) if((Cond)!=true) throw eIllegalOperation(ErrorMessage("ASSERT condition was rised",FILE_NAME,__LINE__))
#else
#define ASSERTPDD(Cond)
#endif

