#pragma once
class npUserData
{
	void* m_data = 0;
public:
	npUserData() {}
	virtual  ~npUserData() {}

	virtual void* GetUserData() { return m_data; }
	virtual void SetUserData(void* d) { m_data = d; }
};

