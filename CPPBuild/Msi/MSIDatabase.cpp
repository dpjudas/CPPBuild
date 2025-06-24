
#include "Precomp.h"
#include "MSIDatabase.h"
#include "IOData/FilePath.h"
#include "IOData/MemoryDevice.h"
#include "Guid/Guid.h"
#include <optional>
#include <strsafe.h>
#include <io.h>
#include <fcntl.h>

std::unique_ptr<MSIDatabase> MSIDatabase::createAlways(const std::string& filename)
{
	return std::make_unique<MSIDatabase>(filename, MSIDBOPEN_CREATE, true);
}

std::unique_ptr<MSIDatabase> MSIDatabase::openExisting(const std::string& filename)
{
	return std::make_unique<MSIDatabase>(filename, MSIDBOPEN_TRANSACT, false);
}

MSIDatabase::MSIDatabase(const std::string& filename, LPCWSTR szPersist, bool needsCommit) : filename(filename), committed(!needsCommit)
{
	UINT result = MsiOpenDatabase(to_utf16(filename).c_str(), szPersist, &handle);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("Could not open MSI database '" + filename + "'");
}

MSIDatabase::~MSIDatabase()
{
	MsiCloseHandle(handle);
	if (!committed)
		File::tryDelete(filename);
}

std::unique_ptr<MSIDatabaseView> MSIDatabase::createView(const std::string& query, int params)
{
	return std::make_unique<MSIDatabaseView>(handle, query, params);
}

std::unique_ptr<MSISummaryInfo> MSIDatabase::getSummaryInfo(int updateCount)
{
	return std::make_unique<MSISummaryInfo>(handle, updateCount);
}

void MSIDatabase::commit()
{
	UINT result = MsiDatabaseCommit(handle);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiDatabaseCommit failed");
	committed = true;
}

void MSIDatabase::importTable(const std::string& folder, const std::string& filename)
{
	UINT result = MsiDatabaseImport(handle, to_utf16(folder).c_str(), to_utf16(filename).c_str());
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiDatabaseImport failed");
}

void MSIDatabase::exportTable(const std::string& tableName, const std::string& folder, const std::string& filename)
{
	UINT result = MsiDatabaseExport(handle, to_utf16(tableName).c_str(), to_utf16(folder).c_str(), to_utf16(filename).c_str());
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiDatabaseExport failed");
}

std::vector<MSIColumnInfo> MSIDatabase::getTableColumnInfo(const std::string& tableName)
{
	auto view = createView("SELECT * From `" + tableName + "`", 0);

	auto record = view->getColumnInfo(MSICOLINFO_NAMES);
	int count = record->getFieldCount();
	std::vector<MSIColumnInfo> columns(count);
	for (int i = 0; i < count; i++)
	{
		columns[i].name = record->getString(i);
	}

	record = view->getColumnInfo(MSICOLINFO_TYPES);
	if (count != record->getFieldCount())
		throw std::runtime_error("MSICOLINFO_NAMES and MSICOLINFO_TYPES returned different field counts");
	for (int i = 0; i < count; i++)
	{
		columns[i].type = record->getString(i);
	}

	record = getPrimaryKeys(tableName);
	count = record->getFieldCount();
	for (int i = 0; i < count; i++)
	{
		std::string colName = record->getString(i);
		for (auto& col : columns)
		{
			if (col.name == colName)
			{
				col.primaryKey = true;
				break;
			}
		}
	}

	return columns;
}

std::map<std::string, std::vector<MSIColumnInfo>> MSIDatabase::getSchema()
{
	std::map<std::string, std::vector<MSIColumnInfo>> tables;

	auto view = createView("SELECT Name From _Tables", 0);
	auto row = view->executeReader();
	while (row->fetch())
	{
		tables[row->getString(0)] = {};
	}
	row.reset();
	view.reset();

	for (auto& it : tables)
	{
		it.second = getTableColumnInfo(it.first);
	}

	return tables;
}

std::unique_ptr<MSIDatabaseRow> MSIDatabase::getPrimaryKeys(const std::string& tableName)
{
	MSIHANDLE record = {};
	UINT result = MsiDatabaseGetPrimaryKeys(handle, to_utf16(tableName).c_str(), &record);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiDatabaseGetPrimaryKeys failed for table: " + tableName);
	return std::make_unique<MSIDatabaseRow>(0, record);
}

/////////////////////////////////////////////////////////////////////////////

MSIDatabaseView::MSIDatabaseView(MSIHANDLE db, const std::string& query, int params) : query(query)
{
	UINT result = MsiDatabaseOpenView(db, to_utf16(query).c_str(), &handle);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("Could not create MSI database view for query: " + query);

	if (params > 0)
	{
		record = MsiCreateRecord(params);
		if (record == 0)
		{
			MsiCloseHandle(handle);
			throw std::runtime_error("MsiCreateRecord failed");
		}
	}
}

MSIDatabaseView::~MSIDatabaseView()
{
	if (record != 0)
		MsiCloseHandle(record);
	MsiCloseHandle(handle);
}

void MSIDatabaseView::setInteger(int field, int value)
{
	UINT result = MsiRecordSetInteger(record, field + 1, value);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiRecordSetInteger failed");
}

void MSIDatabaseView::setString(int field, const std::string& text)
{
	UINT result = MsiRecordSetString(record, field + 1, to_utf16(text).c_str());
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiRecordSetString failed");
}

void MSIDatabaseView::setStream(int field, const std::string& filename)
{
	UINT result = MsiRecordSetStream(record, field + 1, to_utf16(filename).c_str());
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiRecordSetStream failed");
}

void MSIDatabaseView::setStream(int field, const MSIObject& obj)
{
	if (!obj.filename.empty())
	{
		setStream(field, obj.filename);
	}
	else
	{
		// Special thanks to Microsoft for not making MsiRecordSetStreamFromData(record, field, data, size)
		wchar_t buffer[1024];
		DWORD result = GetTempPath2(1024, buffer);
		if (result == 0 || result >= 1024)
			throw std::runtime_error("GetTempPath2 failed");
		std::string filename = FilePath::combine(from_utf16(buffer), Guid::makeGuid().toString()) + ".msistream";
		File::writeAllBytes(filename, obj.data);
		try
		{
			setStream(field, filename);
		}
		catch (...)
		{
			File::tryDelete(filename);
			throw;
		}
		File::tryDelete(filename);
	}
}

void MSIDatabaseView::execute()
{
	UINT result = MsiViewExecute(handle, record);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("Could not execute MSI database view for query: " + query);
	MsiViewClose(handle);
}

std::unique_ptr<MSIDatabaseRow> MSIDatabaseView::executeReader()
{
	UINT result = MsiViewExecute(handle, record);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("Could not execute MSI database view for query: " + query);
	return std::make_unique<MSIDatabaseRow>(handle, 0);
}

std::unique_ptr<MSIDatabaseRow> MSIDatabaseView::getColumnInfo(MSICOLINFO colInfo)
{
	MSIHANDLE record = {};
	UINT result = MsiViewGetColumnInfo(handle, colInfo, &record);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiViewGetColumnInfo failed for query: " + query);
	return std::make_unique<MSIDatabaseRow>(0, record);
}

/////////////////////////////////////////////////////////////////////////////

MSIDatabaseRow::MSIDatabaseRow(MSIHANDLE view, MSIHANDLE record) : view(view), record(record)
{
}

MSIDatabaseRow::~MSIDatabaseRow()
{
	if (record != 0)
		MsiCloseHandle(record);
	if (view != 0)
		MsiViewClose(view);
}

bool MSIDatabaseRow::fetch()
{
	if (view == 0)
		return false;

	if (record != 0)
		MsiCloseHandle(record);
	record = 0;

	MSIHANDLE row = {};
	UINT result = MsiViewFetch(view, &record);
	if (result == ERROR_NO_MORE_ITEMS)
	{
		if (view != 0)
			MsiViewClose(view);
		return false;
	}
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiViewFetch failed");
	return true;
}

int MSIDatabaseRow::getFieldCount()
{
	return MsiRecordGetFieldCount(record);
}

bool MSIDatabaseRow::isNull(int field)
{
	return MsiRecordIsNull(record, field + 1);
}

int MSIDatabaseRow::getInteger(int field)
{
	return MsiRecordGetInteger(record, field + 1);
}

std::string MSIDatabaseRow::getString(int field)
{
	wchar_t empty = 0;
	DWORD size = 1;
	UINT result = MsiRecordGetString(record, field + 1, &empty, &size);
	if (result == ERROR_SUCCESS)
		return {};
	if (result != ERROR_MORE_DATA)
		throw std::runtime_error("MsiRecordGetString failed");

	std::wstring buffer(size, L'\0');
	size++;
	result = MsiRecordGetString(record, field + 1, buffer.data(), &size);
	if (result == ERROR_MORE_DATA)
		throw std::runtime_error("MsiRecordGetString size query failed");
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiRecordGetString failed");
	buffer.resize(size);
	return from_utf16(buffer);
}

std::shared_ptr<DataBuffer> MSIDatabaseRow::getStream(int field)
{
	DWORD size = 0;
	UINT result = MsiRecordReadStream(record, field + 1, nullptr, &size);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiRecordReadStream failed");
	if (size == 0)
		return DataBuffer::create(0);

	auto buffer = DataBuffer::create(size);
	result = MsiRecordReadStream(record, field + 1, buffer->data(), &size);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiRecordReadStream failed");
	buffer->setSize(size);
	return buffer;
}

/////////////////////////////////////////////////////////////////////////////

MSISummaryInfo::MSISummaryInfo(const std::string& filename, int updateCount)
{
	UINT result = MsiGetSummaryInformation(0, to_utf16(filename).c_str(), updateCount, &handle);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiGetSummaryInformation failed");
}

MSISummaryInfo::MSISummaryInfo(MSIHANDLE db, int updateCount)
{
	UINT result = MsiGetSummaryInformation(db, nullptr, updateCount, &handle);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiGetSummaryInformation failed");
}

MSISummaryInfo::~MSISummaryInfo()
{
	MsiCloseHandle(handle);
}

void MSISummaryInfo::commit()
{
	UINT result = MsiSummaryInfoPersist(handle);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiSummaryInfoPersist failed");
}

void MSISummaryInfo::setPropertyStr(UINT property, const std::string& value)
{
	strValues.push_back(std::make_unique<std::wstring>(to_utf16(value)));
	setProperty(property, VT_LPSTR, 0, nullptr, strValues.back()->c_str());
}

void MSISummaryInfo::setProperty(UINT property, UINT dataType, INT iValue, FILETIME* filetimeValue, LPCWSTR strValue)
{
	UINT result = MsiSummaryInfoSetProperty(handle, property, dataType, iValue, filetimeValue, strValue);
	if (result != ERROR_SUCCESS)
		throw std::runtime_error("MsiSummaryInfoSetProperty failed for property " + std::to_string(property));
}

/////////////////////////////////////////////////////////////////////////////

// The table schema is only shipped with the Windows SDK in a Schema.msi file (thanks Microsoft).
// This function extracts the schema for all the tables so we can create them.
// It also extracts the _Validation table contents as Orca needs that for validating a MSI database.
// The output of this function is the MSISchema.h header file. Run it again if Microsoft adds new features to MSI.
std::string MSISchema::generateCode(const std::string& schemaMsi)
{
	auto db = MSIDatabase::openExisting(schemaMsi);

	struct ValidationRule
	{
		std::string table;
		std::string column;
		std::string nullable;
		std::optional<int> minValue;
		std::optional<int> maxValue;
		std::optional<std::string> keyTable;
		std::optional<int> keyColumn;
		std::optional<std::string> category;
		std::optional<std::string> set;
		std::optional<std::string> description;
	};
	std::map<std::string, std::vector<ValidationRule>> tableValidation;

	auto view = db->createView("SELECT `Table`, `Column`, `Nullable`, `MinValue`, `MaxValue`, `KeyTable`, `KeyColumn`, `Category`, `Set`, `Description` FROM `_Validation`", 0);
	auto row = view->executeReader();
	while (row->fetch())
	{
		ValidationRule rule;
		rule.table = row->getString(0);
		rule.column = row->getString(1);
		rule.nullable = row->getString(2);
		if (!row->isNull(3))
			rule.minValue = row->getInteger(3);
		if (!row->isNull(4))
			rule.maxValue = row->getInteger(4);
		if (!row->isNull(5))
			rule.keyTable = row->getString(5);
		if (!row->isNull(6))
			rule.keyColumn = row->getInteger(6);
		if (!row->isNull(7))
			rule.category = row->getString(7);
		if (!row->isNull(8))
			rule.set = row->getString(8);
		if (!row->isNull(9))
			rule.description = row->getString(9);
		std::string tableName = rule.table;
		tableValidation[tableName].push_back(std::move(rule));
	}
	row.reset();
	view.reset();

#ifdef WIN32
	std::string newline = "\r\n";
#else
	std::string newline = "\n";
#endif

	std::string result = "#pragma once" + newline;
	result += newline;
	result += "//" + newline;
	result += "// Warning: this code has been autogenerated by the MSISchema::generateCode function" + newline;
	result += "//" + newline;
	result += newline;
	result += "#include <vector>" + newline;
	result += "#include <string>" + newline;
	result += "#include <optional>" + newline;
	result += "#include <cstdint>" + newline;
	result += "#include \"MSIDatabase.h\"" + newline;
	result += newline;

	std::map<std::string, std::vector<MSIColumnInfo>> tables = db->getSchema();
	for (const auto& it : tables)
	{
		std::string tableName = it.first;
		std::string sql = "CREATE TABLE `" + tableName + "` (";
		std::string primaryKeySql;

		std::string className;
		if (tableName == "_Validation") // This isn't actually an internal table...
			className = "MSIValidation";
		else if (tableName.substr(0, 3) == "Msi") // Newer tables have the Msi prefix. Older tables do not
			className = "MSI" + tableName.substr(3);
		else
			className = "MSI" + tableName;

		std::string classCode = "class " + className + newline + "{" + newline + "public:" + newline;
		std::string createTableCode = "\tstatic void createTable(MSIDatabase* db, const std::vector<" + className + ">& rows)" + newline + "\t{" + newline;

		createTableCode += "\t\tdb->createView(createTableSql(), 0)->execute();" + newline;
		if (tableName != "_Validation")
			createTableCode += "\t\tinsertValidationRules(db);" + newline;

		createTableCode += "\t\tauto view = db->createView(\"INSERT INTO `" + tableName + "` (";
		bool firstColumn = true;
		for (const MSIColumnInfo& column : it.second)
		{
			if (firstColumn)
				firstColumn = false;
			else
				createTableCode += ", ";
			createTableCode += "`" + column.name + "`";
		}
		createTableCode += ") VALUES(";
		firstColumn = true;
		for (const MSIColumnInfo& column : it.second)
		{
			if (firstColumn)
				firstColumn = false;
			else
				createTableCode += ", ";
			createTableCode += "?";
		}
		createTableCode += ")\", " + std::to_string(it.second.size()) + ");" + newline;

		createTableCode += "\t\tfor (auto& row : rows)" + newline;
		createTableCode += "\t\t{" + newline;

		firstColumn = true;
		int index = 0;
		for (const MSIColumnInfo& column : it.second)
		{
			if (column.name.size() < 1)
				throw std::runtime_error("Invalid table column name");
			if (column.type.size() < 2)
				throw std::runtime_error("Unsupported table column type");

			if (firstColumn)
				firstColumn = false;
			else
				sql += ", ";

			sql += "`" + column.name + "` ";

			std::string cppName = column.name;
			if (cppName[0] >= 'A' && cppName[0] <= 'Z')
				cppName[0] = cppName[0] - 'A' + 'a';
			if (cppName.back() == '_') // We don't care about foreign keys in the struct as we don't enforce any of that anyway
				cppName.pop_back();

			// Check for C++ keywords
			if (cppName == "template" || cppName == "class")
				cppName.push_back('_');

			std::string prefix = column.type.substr(0, 2);

			bool notNull = true;
			if (prefix[0] >= 'A' && prefix[0] <= 'Z')
			{
				notNull = false;
				prefix[0] = prefix[0] - 'A' + 'a';
			}

			bool localizable = false;
			if (prefix == "s0")
			{
				sql += "LONGCHAR";
				if (notNull)
				{
					classCode += "\tstd::string " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + ");" + newline;
				}
				else
				{
					classCode += "\tstd::optional<std::string> " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + " ? row." + cppName + ".value() : \"\");" + newline;
				}
			}
			else if (prefix == "l0")
			{
				localizable = true;
				sql += "LONGCHAR";
				if (notNull)
				{
					classCode += "\tstd::string " + cppName + "; // localizable" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + ");" + newline;
				}
				else
				{
					classCode += "\tstd::optional<std::string> " + cppName + "; // localizable" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + " ? row." + cppName + ".value() : \"\");" + newline;
				}
			}
			else if (prefix == "i2")
			{
				sql += "SHORT";
				if (notNull)
				{
					classCode += "\tint " + cppName + " = 0;" + newline;
					createTableCode += "\t\t\tview->setInteger(" + std::to_string(index) + ", row." + cppName + ");" + newline;
				}
				else
				{
					classCode += "\tstd::optional<int> " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setInteger(" + std::to_string(index) + ", row." + cppName + " ? row." + cppName + ".value() : MSI_NULL_INTEGER);" + newline;
				}
			}
			else if (prefix == "i4")
			{
				sql += "LONG";
				if (notNull)
				{
					classCode += "\tint " + cppName + " = 0;" + newline;
					createTableCode += "\t\t\tview->setInteger(" + std::to_string(index) + ", row." + cppName + ");" + newline;
				}
				else
				{
					classCode += "\tstd::optional<int> " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setInteger(" + std::to_string(index) + ", row." + cppName + " ? row." + cppName + ".value() : MSI_NULL_INTEGER);" + newline;
				}
			}
			else if (prefix == "v0")
			{
				sql += "OBJECT";
				if (notNull)
				{
					classCode += "\tMSIObject " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setStream(" + std::to_string(index) + ", row." + cppName + ");" + newline;
				}
				else
				{
					classCode += "\tstd::optional<MSIObject> " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setStream(" + std::to_string(index) + ", row." + cppName + " ? row." + cppName + ".value() : MSIObject());" + newline;
				}
			}
			else if (prefix[0] == 's')
			{
				sql += "CHAR(" + column.type.substr(1) + ")";
				if (notNull)
				{
					classCode += "\tstd::string " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + ");" + newline;
				}
				else
				{
					classCode += "\tstd::optional<std::string> " + cppName + ";" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + " ? row." + cppName + ".value() : \"\");" + newline;
				}
			}
			else if (prefix[0] == 'l')
			{
				localizable = true;
				sql += "CHAR(" + column.type.substr(1) + ")";
				if (notNull)
				{
					classCode += "\tstd::string " + cppName + "; // localizable" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + ");" + newline;
				}
				else
				{
					classCode += "\tstd::optional<std::string> " + cppName + "; // localizable" + newline;
					createTableCode += "\t\t\tview->setString(" + std::to_string(index) + ", row." + cppName + " ? row." + cppName + ".value() : \"\");" + newline;
				}
			}
			else if (prefix[0] == 'g' || prefix[0] == 'j' || prefix[0] == 'O')
			{
				throw std::runtime_error("Unsupported temporary fields");
			}
			else
			{
				throw std::runtime_error("Unsupported table column type");
			}

			if (notNull)
				sql += " NOT NULL";
			if (localizable)
				sql += " LOCALIZABLE";

			if (column.primaryKey)
			{
				if (primaryKeySql.empty())
					primaryKeySql += " PRIMARY KEY `" + column.name + "`";
				else
					primaryKeySql += ", `" + column.name + "`";
			}

			index++;
		}

		sql += primaryKeySql;
		sql += ")";

		createTableCode += "\t\t\tview->execute();" + newline;
		createTableCode += "\t\t}" + newline;
		createTableCode += "\t}" + newline;

		std::string validationCode;
		if (tableName != "_Validation")
		{
			validationCode = "\tstatic void insertValidationRules(MSIDatabase* db)" + newline + "\t{" + newline;
			validationCode += "\t\tauto view = db->createView(\"INSERT INTO `_Validation` (`Table`, `Column`, `Nullable`, `MinValue`, `MaxValue`, `KeyTable`, `KeyColumn`, `Category`, `Set`, `Description`) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)\", 10);" + newline;
			for (auto& rule : tableValidation[tableName])
			{
				validationCode += "\t\tview->setString(0, \"" + escapeString(rule.table) + "\");" + newline;
				validationCode += "\t\tview->setString(1, \"" + escapeString(rule.column) + "\");" + newline;
				validationCode += "\t\tview->setString(2, \"" + escapeString(rule.nullable) + "\");" + newline;
				validationCode += "\t\tview->setInteger(3, " + (rule.minValue ? std::to_string(rule.minValue.value()) : "MSI_NULL_INTEGER") + ");" + newline;
				validationCode += "\t\tview->setInteger(4, " + (rule.maxValue ? std::to_string(rule.maxValue.value()) : "MSI_NULL_INTEGER") + ");" + newline;
				validationCode += "\t\tview->setString(5, \"" + escapeString(rule.keyTable ? rule.keyTable.value() : "") + "\");" + newline;
				validationCode += "\t\tview->setInteger(6, " + (rule.keyColumn ? std::to_string(rule.keyColumn.value()) : "MSI_NULL_INTEGER") + ");" + newline;
				validationCode += "\t\tview->setString(7, \"" + escapeString(rule.category ? rule.category.value() : "") + "\");" + newline;
				validationCode += "\t\tview->setString(8, \"" + escapeString(rule.set ? rule.set.value() : "") + "\");" + newline;
				validationCode += "\t\tview->setString(9, \"" + escapeString(rule.description ? rule.description.value() : "") + "\");" + newline;
				validationCode += "\t\tview->execute();" + newline;
			}
			validationCode += "\t}" + newline;
		}

		classCode += newline + "\tstatic std::string createTableSql() " + newline + "\t{" + newline + "\t\treturn \"" + sql + "\";" + newline + "\t}" + newline;
		classCode += newline;
		classCode += validationCode;
		classCode += createTableCode;
		classCode += "};" + newline + newline;

		result += classCode;
	}

	return result;
}

std::string MSISchema::escapeString(const std::string& v)
{
	std::string r;
	r.reserve(v.size());
	for (char c : v)
	{
		if (c == '"' || c == '\\')
			r.push_back('\\');
		r.push_back(c);
	}
	return r;
}

// Exports a MSI database in C++ form to get the standard dialogs out of UISample.msi (thanks Microsoft)
std::string MSISchema::exportTables(const std::string& msi)
{
	auto db = MSIDatabase::openExisting(msi);

#ifdef WIN32
	std::string newline = "\r\n";
#else
	std::string newline = "\n";
#endif

	std::string result;

	std::map<std::string, std::vector<MSIColumnInfo>> tables = db->getSchema();
	for (const auto& it : tables)
	{
		const std::string& tableName = it.first;
		const std::vector<MSIColumnInfo>& columns = it.second;

		std::string className;
		if (tableName == "_Validation") // This isn't actually an internal table...
			className = "MSIValidation";
		else if (tableName.substr(0, 3) == "Msi") // Newer tables have the Msi prefix. Older tables do not
			className = "MSI" + tableName.substr(3);
		else
			className = "MSI" + tableName;

		std::string listVarName = className.substr(3);
		if (listVarName[0] >= 'A' && listVarName[0] <= 'Z')
			listVarName[0] = listVarName[0] - 'A' + 'a';

		result += "std::vector<" + className + "> " + listVarName + " = " + newline + "{" + newline;

		std::string selectSQL = "SELECT ";
		bool firstColumn = true;
		for (const MSIColumnInfo& column : columns)
		{
			if (firstColumn)
				firstColumn = false;
			else
				selectSQL += ", ";
			selectSQL += "`";
			selectSQL += column.name;
			selectSQL += "`";
		}
		selectSQL += "FROM `" + tableName + "`";

		auto view = db->createView(selectSQL, 0);
		auto row = view->executeReader();
		while (row->fetch())
		{
			std::string rowCode = "\t{ ";

			int field = 0;
			bool firstField = true;
			for (const MSIColumnInfo& column : columns)
			{
				if (column.name.size() < 1)
					throw std::runtime_error("Invalid table column name");
				if (column.type.size() < 2)
					throw std::runtime_error("Unsupported table column type");

				std::string cppName = column.name;
				if (cppName[0] >= 'A' && cppName[0] <= 'Z')
					cppName[0] = cppName[0] - 'A' + 'a';
				if (cppName.back() == '_') // We don't care about foreign keys in the struct as we don't enforce any of that anyway
					cppName.pop_back();

				// Check for C++ keywords
				if (cppName == "template" || cppName == "class")
					cppName.push_back('_');

				std::string prefix = column.type.substr(0, 2);
				bool notNull = true;
				if (prefix[0] >= 'A' && prefix[0] <= 'Z')
				{
					notNull = false;
					prefix[0] = prefix[0] - 'A' + 'a';
				}

				std::string fieldCode;
				if (notNull || !row->isNull(field))
				{
					bool localizable = false;
					if (prefix[0] == 's' || prefix[0] == 'l')
					{
						std::string value = row->getString(field);
						fieldCode = "." +cppName + " = \"" + escapeString(value) + "\"";
					}
					else if (prefix[0] == 'i')
					{
						fieldCode = "." + cppName + " = " + std::to_string(row->getInteger(field));
					}
					else if (prefix == "v0")
					{
						//fieldCode = "." + cppName + ".data = std::vector<uint8_t>(" + row->getStream(field) + ")";
					}
					else if (prefix[0] == 'g' || prefix[0] == 'j' || prefix[0] == 'O')
					{
						throw std::runtime_error("Unsupported temporary fields");
					}
					else
					{
						throw std::runtime_error("Unsupported table column type");
					}
				}
				
				if (!fieldCode.empty())
				{
					if (firstField)
						firstField = false;
					else
						rowCode += ", ";
					rowCode += fieldCode;
				}

				field++;
			}

			rowCode += " }," + newline;
			result += rowCode;
		}

		result += "};" + newline;
		result += newline;
	}

	return result;
}

void MSISchema::saveBinaries(const std::string& msi, const std::string& outputFolder)
{
	auto db = MSIDatabase::openExisting(msi);
	auto view = db->createView("SELECT Name, Data FROM Binary", 0);
	auto row = view->executeReader();
	while (row->fetch())
	{
		std::string name = row->getString(0);
		auto data = row->getStream(1);
		File::writeAllBytes(FilePath::combine(outputFolder, name), data);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CabinetWriter
// 
// Wow, what a garbage API from Microsoft. Doesn't even do unicode it seems.
// We are going to pretend the ANSI codepage is CP_UTF8.

struct CabinetWriter::Impl
{
	std::map<std::string, std::shared_ptr<MemoryDevice>> files;
	int nextTempFilename = 0;
	std::vector<std::shared_ptr<IODevice>> openFiles;

	INT_PTR openExisting(const char* filename)
	{
		auto it = files.find(filename);
		if (it == files.end())
			return createFileIndex(File::openExisting(filename));
		else
			return createFileIndex(it->second);
	}

	INT_PTR createAlways(const char* filename)
	{
		auto& file = files[filename];
		file = MemoryDevice::create();
		return createFileIndex(file);
	}

	void closeFile(INT_PTR index)
	{
		openFiles[index - 1].reset();
	}

	IODevice* getFile(INT_PTR index)
	{
		return openFiles[index - 1].get();
	}

	INT_PTR createFileIndex(std::shared_ptr<IODevice> file)
	{
		for (size_t i = 0; i < openFiles.size(); i++)
		{
			if (!openFiles[i])
			{
				openFiles[i] = std::move(file);
				return i + 1;
			}
		}

		INT_PTR index = (INT_PTR)openFiles.size();
		openFiles.push_back(std::move(file));
		return index + 1;
	}

	void deleteFile(const char* filename)
	{
		auto it = files.find(filename);
		if (it != files.end())
			files.erase(it);
	}

	std::string getTempFilename()
	{
		return "tempfile-" + std::to_string(nextTempFilename++) + ".tmp";
	}
};

static FNFCIFILEPLACED(fnFilePlaced)
{
	return 0;
}

static FNFCIOPEN(fnFileOpen)
{
	UNREFERENCED_PARAMETER(pmode);
	try
	{
		auto writer = static_cast<CabinetWriter::Impl*>(pv);
		if (oflag & _O_CREAT)
			return writer->createAlways(pszFile);
		else
			return writer->openExisting(pszFile);
	}
	catch (...)
	{
		*err = ERROR_FILE_NOT_FOUND;
		return -1;
	}
}

static FNFCIREAD(fnFileRead)
{
	try
	{
		auto writer = static_cast<CabinetWriter::Impl*>(pv);
		return (UINT)writer->getFile(hf)->try_read(memory, cb);
	}
	catch (...)
	{
		*err = ERROR_READ_FAULT;
		return -1;
	}
}

static FNFCIWRITE(fnFileWrite)
{
	try
	{
		auto writer = static_cast<CabinetWriter::Impl*>(pv);
		writer->getFile(hf)->write(memory, cb);
		return cb;
	}
	catch (...)
	{
		*err = ERROR_WRITE_FAULT;
		return -1;
	}
}

static FNFCICLOSE(fnFileClose)
{
	auto writer = static_cast<CabinetWriter::Impl*>(pv);
	writer->closeFile(hf);
	return 0;
}

static FNFCISEEK(fnFileSeek)
{
	try
	{
		auto writer = static_cast<CabinetWriter::Impl*>(pv);
		if (seektype == FILE_BEGIN)
			return (long)writer->getFile(hf)->seek(dist);
		else if (seektype == FILE_CURRENT)
			return (long)writer->getFile(hf)->seek_from_current(dist);
		else if (seektype == FILE_END)
			return (long)writer->getFile(hf)->seek_from_end(dist);
		else
			throw std::runtime_error("Unknown seek value");
	}
	catch (...)
	{
		*err = ERROR_SEEK;
		return -1;
	}
}

static FNFCIDELETE(fnFileDelete)
{
	*err = ERROR_SUCCESS;
	auto writer = static_cast<CabinetWriter::Impl*>(pv);
	writer->deleteFile(pszFile);
	return 0;
}

static FNFCIGETTEMPFILE(fnGetTempFileName)
{
	auto writer = static_cast<CabinetWriter::Impl*>(pv);
	std::string filename = writer->getTempFilename();
	return SUCCEEDED(StringCbCopyA(pszTempName, cbTempName, filename.c_str()));
}

static FNFCIGETOPENINFO(fnGetOpenInfo)
{
	HANDLE hFile = CreateFile(to_utf16(pszName).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		*err = ERROR_FILE_NOT_FOUND;
		return -1;
	}

	FILETIME fileTime;
	BY_HANDLE_FILE_INFORMATION fileInfo;
	if (GetFileInformationByHandle(hFile, &fileInfo) && FileTimeToLocalFileTime(&fileInfo.ftCreationTime, &fileTime) && FileTimeToDosDateTime(&fileTime, pdate, ptime))
	{
		*pattribs = (USHORT)fileInfo.dwFileAttributes;
		*pattribs &= (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH);
		CloseHandle(hFile);
	}
	else
	{
		CloseHandle(hFile);
		*err = ERROR_FILE_NOT_FOUND;
		return -1;
	}

	return fnFileOpen(pszName, _O_RDONLY, 0, err, pv);
}

static FNFCIALLOC(fnMemAlloc)
{
	return malloc(cb);
}

static FNFCIFREE(fnMemFree)
{
	free(memory);
}

static FNFCIGETNEXTCABINET(fnGetNextCabinet)
{
	return FALSE;
	/*
	HRESULT hr;

	UNREFERENCED_PARAMETER(pv);
	UNREFERENCED_PARAMETER(cbPrevCab);

	hr = StringCchPrintfA(pccab->szCab,
		ARRAYSIZE(pccab->szCab),
		"FCISample%02d.cab",
		pccab->iCab);

	return (SUCCEEDED(hr));
	*/
}

static FNFCISTATUS(fnStatus)
{
	return 0;
}

CabinetWriter::CabinetWriter() : pimpl(new Impl())
{
	/*
	wchar_t buffer[1024];
	DWORD result = GetTempPath2(1024, buffer);
	if (result == 0 || result >= 1024)
		throw std::runtime_error("GetTempPath2 failed");
	filename = FilePath::combine(from_utf16(buffer), Guid::makeGuid().toString()) + ".cab";
	*/

	filename = "__cabinet__.cab";

	std::string tempDisk;
	std::string tempPath = FilePath::removeLastComponent(filename);
	std::string tempFilename = FilePath::lastComponent(filename);

	if (tempDisk.size() >= CB_MAX_DISK_NAME || tempFilename.size() >= CB_MAX_CABINET_NAME || tempPath.size() >= CB_MAX_CAB_PATH)
		throw std::runtime_error("Filename too long for the cabinet writer");

	CCAB cabinfo = {};
	cabinfo.cb = 0x7fffffff;
	cabinfo.cbFolderThresh = 0x7fffffff;
	cabinfo.setID = 1;
	cabinfo.iCab = 1;
	cabinfo.iDisk = 0;
	strcpy_s(cabinfo.szDisk, tempDisk.c_str());
	strcpy_s(cabinfo.szCab, tempFilename.c_str());
	strcpy_s(cabinfo.szCabPath, tempPath.c_str());

	ERF errorInfo = {};
	handle = FCICreate(
		&errorInfo,
		fnFilePlaced,
		fnMemAlloc,
		fnMemFree,
		fnFileOpen,
		fnFileRead,
		fnFileWrite,
		fnFileClose,
		fnFileSeek,
		fnFileDelete,
		fnGetTempFileName,
		&cabinfo,
		pimpl.get());

	if (!handle)
	{
		File::tryDelete(filename);
		throw std::runtime_error("FCICreate failed");
	}
}

CabinetWriter::~CabinetWriter()
{
	if (!closed)
	{
		FCIDestroy(handle);
		File::tryDelete(filename);
	}
}

void CabinetWriter::addFile(const std::string& filename, const std::string& sourceFile, bool executeFlag)
{
	BOOL result = FCIAddFile(
		handle,
		const_cast<LPSTR>(sourceFile.c_str()),
		const_cast<LPSTR>(filename.c_str()),
		executeFlag ? TRUE : FALSE,
		fnGetNextCabinet,
		fnStatus,
		fnGetOpenInfo,
		tcompTYPE_MSZIP);
	if (result == FALSE)
		throw std::runtime_error("FCIAddFile failed");
}

std::shared_ptr<DataBuffer> CabinetWriter::close()
{
	if (closed)
		throw std::runtime_error("Cabinet already closed");

	BOOL result = FCIFlushCabinet(handle, FALSE, fnGetNextCabinet, fnStatus);
	if (result == FALSE)
		throw std::runtime_error("FCIFlushCabinet failed");

	FCIDestroy(handle);
	closed = true;

	//auto data = File::readAllBytes(filename);
	//File::tryDelete(filename);
	return pimpl->files[filename]->buffer();
}
