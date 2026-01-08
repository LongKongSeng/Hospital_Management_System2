#ifndef ID_FORMATTER_H
#define ID_FORMATTER_H

#include <string>
#include <mysql/jdbc.h>

using namespace std;

// Helper function to safely get formatted_id or fallback to numeric ID as string
static string getFormattedId(sql::ResultSet* res, const string& formattedIdColumn, const string& numericIdColumn) {
    if (res->isNull(formattedIdColumn)) {
        return to_string(res->getInt(numericIdColumn));
    } else {
        return string(res->getString(formattedIdColumn));
    }
}

#endif

