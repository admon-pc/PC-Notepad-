#pragma once
class npID
{
	uint32_t m_id = 0;
public:
	npID() {}
	virtual  ~npID() {}

	virtual uint32_t GetID() { return m_id; }
	virtual void SetID(uint32_t newID) { m_id = newID; }
};

