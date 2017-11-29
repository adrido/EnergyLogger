#include "Database.h"

using namespace std;

Database::Database()
{
	try {
		/* Create a connection */
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "pi", "123456");
		/* Connect to the MySQL test database */
		con->setSchema("default");

		/* Prepare the insert statement */
		stmt = con->prepareStatement("INSERT INTO Messdaten(zeitstempel, u1, u2, u3) VALUES (NOW(), ?, ?, ?)");
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
}


Database::~Database()
{
}

bool Database::save(float u1, float u2, float u3)
{
	try {
		stmt->setDouble(1, u1);
		stmt->setDouble(2, u2);
		stmt->setDouble(3, u3);
		stmt->execute();
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line "
			<< __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		return false;
	}
	return true;
}
