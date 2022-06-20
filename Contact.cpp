#include "Contact.h"

Contact::Contact() : m_phone("NULL"), m_name("NULL")
{

}

Contact::Contact(std::string phone, std::string name) : m_phone(phone), m_name(name)
{
}

void Contact::serialize(sqlite3* db) const
{
	std::string query = std::format("INSERT INTO contact VALUES ('{}', '{}');", this->m_phone, this->m_name);
	if (sqlite3_exec(db, query.c_str(), 0, 0, 0))
		throw std::runtime_error("Unable to execute SQL query");
}

void Contact::deserialize(sqlite3_stmt* statement)
{
	this->m_phone = (const char*)sqlite3_column_text(statement, 0);
	this->m_name = (const char*)sqlite3_column_text(statement, 1);
}
