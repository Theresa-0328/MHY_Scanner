#include "Sha256.h"
Sha256::Sha256()
{
}
Sha256::Sha256(std::string data1):data(data1)
{
}

std::string Sha256::init()
{
	char c;
	int index = 0;
	for (auto it=data.begin();it!=data.end();it++)
	{
		c = *it;
		index += 8;
		std::vector<bool> now;
		int x = c;
		while (x) 
		{
			if (x & 1) now.push_back(1);
			else now.push_back(0);
			x >>= 1;
		}
		while (now.size() < 8) now.push_back(0);// Added 0
		reverse(now.begin(), now.end());
		for (auto x : now) m_data.push_back(x);
		if (m_data.size() == 512) 
		{// When 512bits
			//has();
			m_data.clear();
		}
	}
	int cnt = m_data.size();
	if (cnt >= 448) cnt = 448 + 512 - cnt;
	else cnt = 448 - cnt;
	m_data.push_back(1);// Added 1
	--cnt;
	return data;
}