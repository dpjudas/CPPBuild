#pragma once

#include "IOData/DataBuffer.h"
#include "IOData/File.h"
#include "Text/UTF16.h"

#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <Windows.h>
#include <MsiQuery.h>

#pragma comment(lib, "Msi.lib")

class MSIColumnInfo;
class MSIDatabaseView;
class MSIDatabaseRow;
class MSISummaryInfo;
class MSIObject;

class MSIDatabase
{
public:
	static std::unique_ptr<MSIDatabase> createAlways(const std::string& filename);
	static std::unique_ptr<MSIDatabase> openExisting(const std::string& filename);

	MSIDatabase(const std::string& filename, LPCWSTR szPersist, bool needsCommit = false);
	~MSIDatabase();

	std::unique_ptr<MSIDatabaseView> createView(const std::string& query, int params);
	std::unique_ptr<MSISummaryInfo> getSummaryInfo(int updateCount);

	void commit();
	void importTable(const std::string& folder, const std::string& filename);
	void exportTable(const std::string& tableName, const std::string& folder, const std::string& filename);

	std::vector<MSIColumnInfo> getTableColumnInfo(const std::string& tableName);
	std::map<std::string, std::vector<MSIColumnInfo>> getSchema();
	std::unique_ptr<MSIDatabaseRow> getPrimaryKeys(const std::string& tableName);

	template<typename T>
	void createTable(const std::vector<T>& rows)
	{
		T::createTable(this, rows);
	}

private:
	std::string filename;
	MSIHANDLE handle = {};
	bool committed = false;
};

class MSIDatabaseView
{
public:
	MSIDatabaseView(MSIHANDLE db, const std::string& query, int params);
	~MSIDatabaseView();

	void setInteger(int field, int value);
	void setString(int field, const std::string& text);
	void setStream(int field, const std::string& filename);
	void setStream(int field, const MSIObject& obj);

	void execute();
	std::unique_ptr<MSIDatabaseRow> executeReader();

	std::unique_ptr<MSIDatabaseRow> getColumnInfo(MSICOLINFO colInfo);

private:
	std::string query;
	MSIHANDLE handle = {};
	MSIHANDLE record = {};
};

class MSIDatabaseRow
{
public:
	MSIDatabaseRow(MSIHANDLE view, MSIHANDLE record);
	~MSIDatabaseRow();

	bool fetch();

	int getFieldCount();
	bool isNull(int field);
	int getInteger(int field);
	std::string getString(int field);
	std::shared_ptr<DataBuffer> getStream(int field);

private:
	MSIHANDLE view = {};
	MSIHANDLE record = {};
};

class MSISummaryInfo
{
public:
	MSISummaryInfo(const std::string& filename, int updateCount);
	MSISummaryInfo(MSIHANDLE db, int updateCount);
	~MSISummaryInfo();

	void setCodepage(int value) { setPropertyInt16(1/*PID_CODEPAGE*/, value); }
	void setTitle(const std::string& value) { setPropertyStr(2/*PID_TITLE*/, value); }
	void setSubject(const std::string& value) { setPropertyStr(3/*PID_SUBJECT*/, value); }
	void setAuthor(const std::string& value) { setPropertyStr(4/*PID_AUTHOR*/, value); }
	void setKeywords(const std::string& value) { setPropertyStr(5/*PID_KEYWORDS*/, value); }
	void setComments(const std::string& value) { setPropertyStr(6/*PID_COMMENTS*/, value); }
	void setTemplate(const std::string& value) { setPropertyStr(7/*PID_TEMPLATE*/, value); }
	void setLastSavedBy(const std::string& value) { setPropertyStr(8/*PID_LASTAUTHOR*/, value); }
	void setRevisionNumber(const std::string& value) { setPropertyStr(9/*PID_REVNUMBER*/, value); }
	void setLastPrinted(FILETIME value) { setPropertyFiletime(11/*PID_LASTPRINTED*/, value); }
	void setCreateDatetime(FILETIME value) { setPropertyFiletime(12/*PID_CREATE_DTM*/, value); }
	void setLastSaveDatetime(FILETIME value) { setPropertyFiletime(13/*PID_LASTSAVE_DTM*/, value); }
	void setPageCount(int value) { setPropertyInt32(14/*PID_PAGECOUNT*/, value); }
	void setWordCount(int value) { setPropertyInt32(15/*PID_WORDCOUNT*/, value); }
	void setCharCount(int value) { setPropertyInt32(16/*PID_CHARCOUNT*/, value); }
	void setCreatingApplication(const std::string& value) { setPropertyStr(18/*PID_APPNAME*/, value); }
	void setSecurity(int value) { setPropertyInt32(19/*PID_SECURITY*/, value); }

	void commit();

private:
	void setPropertyInt16(UINT property, int value) { setProperty(property, VT_I2, value, nullptr, nullptr); }
	void setPropertyInt32(UINT property, int value) { setProperty(property, VT_I4, value, nullptr, nullptr); }
	void setPropertyFiletime(UINT property, FILETIME value) { setProperty(property, VT_FILETIME, 0, &value, nullptr); }
	void setPropertyStr(UINT property, const std::string& value);
	void setProperty(UINT property, UINT dataType, INT iValue, FILETIME* filetimeValue, LPCWSTR strValue);

	MSIHANDLE handle = {};
	std::vector<std::unique_ptr<std::wstring>> strValues;
};

class MSIColumnInfo
{
public:
	std::string name;
	std::string type;
	bool primaryKey = false;
};

class MSISchema
{
public:
	static std::string generateCode(const std::string& schemaMsi);
	static std::string exportTables(const std::string& msi);
	static void saveBinaries(const std::string& msi, const std::string& outputFolder);

private:
	static std::string escapeString(const std::string& str);
};

class MSIObject
{
public:
	MSIObject() = default;
	MSIObject(const std::string& filename) : filename(filename) { }
	MSIObject(const std::shared_ptr<DataBuffer>& data) : data(data) { }

	std::string filename;
	std::shared_ptr<DataBuffer> data;
};
