#pragma once
#include <string>
#include <format>

#include <sqlite/sqlite3.h>

class Serializable
{
public:
	virtual void serialize(sqlite3* db) const = 0;
};

class Deserializable
{
public:
	virtual void deserialize(sqlite3_stmt* statement) = 0;
};

class Contact : public Serializable, Deserializable
{
private:
	std::string m_phone;
	std::string m_name;

public:
	Contact();
	Contact(std::string phone, std::string name);

public:
	void serialize(sqlite3* db) const override;
	void deserialize(sqlite3_stmt* statement);

public:
	inline std::string phone() const { return this->m_phone; }
	inline std::string name() const { return this->m_name; }
};