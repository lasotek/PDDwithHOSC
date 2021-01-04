#include <string>
class _CStrOperIdentifier
{
public:
	~_CStrOperIdentifier()
	{
	}
	operator const string&() {return GetStr();}
protected:
	const string& GetStr();
	virtual void CreateStrIdentyfier(string& StrId)=0;
	string m_StrId;
};
