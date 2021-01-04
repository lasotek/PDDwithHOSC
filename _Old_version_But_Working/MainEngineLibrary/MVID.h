#pragma once
#include <vector>

class MVIDS : public vector<long long>
{
public:
	~MVIDS(void);
	long long GetHash(long long& Core) const;
	long long GetHashLess(long long& Core) const;
	bool AlmostEq(const MVIDS& Right) const;
	long long GetHashRev(long long& Core) const;
	bool RevEq(const MVIDS& Right) const;
	string ToStr() const;
};
