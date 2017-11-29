#pragma once

#include <iostream>

/*
Include directly the different
headers from cppconn/ and mysql_driver.h + mysql_util.h
(and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

class Database
{
public:
	Database();
	~Database();

	bool save(float u1, float u2, float u3);
private:
	sql::Driver *driver;
	sql::Connection *con;

	sql::PreparedStatement  *stmt;
};

