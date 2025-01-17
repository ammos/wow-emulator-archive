#ifndef __DATASTORE_H
#define __DATASTORE_H

#include "Common.h"
#include "Singleton.h"
#include "DBC.h"

template <class T>
class DataStore : public Singleton<DataStore<T> >
{
protected:
	DBC d;
public:
	DataStore(const char* filename)
	{
		d.Load(filename);
	}
	~DataStore() {}

	virtual T *LookupEntry(const uint32 row)
	{
		if(!d.IsLoaded() || (uint32)d.GetRows() < row) return NULL;
		return (T*)d.GetRow(row);
	}
	const char* LookupString(const uint32 offset)
	{
		if(!d.IsLoaded() || (uint32)d.GetDBSize() < offset) return NULL;
		return d.LookupString(offset);
	}
	uint32 GetNumRows()
	{
		return d.GetRows();
	}
};

template <class T>
class IndexedDataStore : public DataStore<T>
{
protected:
	std::map<uint32,uint32> indexMap;
public:
	IndexedDataStore(const char* filename) : DataStore<T>(filename)
	{
		for(uint32 row=0;row<(uint32)DataStore<T>::d.GetRows();row++)
			indexMap[*(int*)DataStore<T>::d.GetRow(row)] = row;
	}
	~IndexedDataStore() {}

	virtual T *LookupEntry(const uint32 row)
	{
		if(!DataStore<T>::d.IsLoaded() || (uint32)DataStore<T>::d.GetRows() < row) return NULL;
		return (T*)DataStore<T>::d.GetRow(indexMap[row]);
	}
};

#define defineDBCStore(name,structname) \
class name : public DataStore<structname> \
{ \
public: \
	name(const char* filename); \
	~name(); \
}

#define implementDBCStore(name,structname) \
initialiseSingleton(name); \
initialiseSingleton(DataStore< structname >); \
name::name(const char* filename) : DataStore<structname>(filename) {} \
name::~name() {} \

#define defineIndexedDBCStore(name,structname) \
class name : public IndexedDataStore<structname> \
{ \
public: \
	name(const char* filename); \
	~name(); \
}

#define implementIndexedDBCStore(name,structname) \
initialiseSingleton(name); \
initialiseSingleton(DataStore< structname >); \
name::name(const char* filename) : IndexedDataStore<structname>(filename) {} \
name::~name() {} 


#endif
