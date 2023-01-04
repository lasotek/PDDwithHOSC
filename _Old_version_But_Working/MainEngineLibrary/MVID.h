#pragma once
#include <vector>

class MVIDS : public vector<long long>
{
public:
	~MVIDS(void);
	size_t GetHash(size_t& Core) const;
	size_t GetHashLess(size_t& Core) const;
	bool AlmostEq(const MVIDS& Right) const;
	size_t GetHashRev(size_t& Core) const;
	bool RevEq(const MVIDS& Right) const;
	string ToStr() const;
};
