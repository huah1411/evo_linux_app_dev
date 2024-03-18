/*------------------------------------------------------------------------------------------------------------*
 *                                                                                                            *
 * Copyright      2020 MiTAC International Corp.                                                              *
 *                                                                                                            *
 *------------------------------------------------------------------------------------------------------------*
 * PROJECT     :  Common Framework                                                                            *
 * BINARY NAME :  libBase                                                                                     *
 * FILE NAME   :  util/SimplePropertySet.h                                                                    *
 * CREATED BY  :  Huah Tu <huah.tu@mic.com.tw>                                                                *
 * CREATED DATE:  06/16/20 (MM/DD/YY)                                                                         *
 * DESCRIPTION :  Offer very simple property object, usually for objects' client data support.                *
 *------------------------------------------------------------------------------------------------------------*/

// TODO: add enumeration with function pointer.

#ifndef _UTIL_SIMPLE_PROPERTY_SET_H
#define _UTIL_SIMPLE_PROPERTY_SET_H

// Standard includes
#include <stdlib.h>
#include <string.h>
#include <unordered_map>

struct CStringHash
{
    std::size_t operator()(const char *key) const
    {
        size_t value = 0;
        const char *ptr = key;
        for(int i = 0; (i < 10) && (*ptr); ++i, ++ptr)
        {
            value += *ptr;
        }
        return value;
    }
};

struct CStringHashEqual
{
    bool operator()(const char *key1, const char *key2) const
    {
        return (strcmp(key1, key2) == 0);
    }
};

// All key and value's lifetime should be managed by clients well.  And usually, keys (name) are usually
// constant string literals, and cannot be deleted or free(), so, it's not a problem for keys.
class SimplePropertySet
{
  public:
    SimplePropertySet(void);

    // Declare it as public, for some performance requirement.
    std::unordered_map<const char *, void *, CStringHash, CStringHashEqual> _properties;

    int size(void);
    void *get(const char *name);
    void set(const char *name, void *value);
    bool remove(const char *name, char **orgNameHolder = 0, void **valueHolder = 0);
    // Nothing is free().
    void reset(void);
    // free() is used, so, all values should be generated by alloc(), malloc(), strdup(), or whatever
    // free()-able functions.
    void freeAllValuesAndReset(void);
    // free() is used, so, all keys and values should be generated by alloc(), malloc(), strdup(), or whatever
    // free()-able functions.
    void freeAllKeysAndValuesAndReset(void);

  private:
    // Declared as private to disallow the access.
    SimplePropertySet(SimplePropertySet &src);
    SimplePropertySet &operator=(SimplePropertySet &src);
};

inline SimplePropertySet::SimplePropertySet(void)
{
}

inline int SimplePropertySet::size(void)
{
    return (int) _properties.size();
}

inline void *SimplePropertySet::get(const char *name)
{
    std::unordered_map<const char *, void *, CStringHash, CStringHashEqual>::const_iterator it =
                                                                                  _properties.find(name);
    if(it == _properties.end())
    {
        return 0;
    }
    return it->second;
}

inline void SimplePropertySet::set(const char *name, void *value)
{
    _properties[name] = value;
}

inline bool SimplePropertySet::remove(const char *name, char **orgNameHolder, void **valueHolder)
{
    auto it = _properties.find(name);
    if(it == _properties.end())
    {
        return false;
    }
    if(orgNameHolder)
    {
        *orgNameHolder = (char *) it->first;
    }
    if(valueHolder)
    {
        *valueHolder = it->second;
    }
    _properties.erase(it);
    return true;
}

inline void SimplePropertySet::reset(void)
{
    _properties.clear();
}

inline void SimplePropertySet::freeAllValuesAndReset(void)
{
    std::unordered_map<const char *, void *, CStringHash, CStringHashEqual>::const_iterator it =
                                                                                           _properties.begin();
    std::unordered_map<const char *, void *, CStringHash, CStringHashEqual>::const_iterator itEnd =
                                                                                             _properties.end();
    for(; it != itEnd; ++it)
    {
        free(it->second);
    }
    _properties.clear();
}

inline void SimplePropertySet::freeAllKeysAndValuesAndReset(void)
{
    std::unordered_map<const char *, void *, CStringHash, CStringHashEqual>::const_iterator it =
                                                                                           _properties.begin();
    std::unordered_map<const char *, void *, CStringHash, CStringHashEqual>::const_iterator itEnd =
                                                                                             _properties.end();
    for(; it != itEnd; ++it)
    {
        free((void *) (it->first));
        free(it->second);
    }
    _properties.clear();
}

#endif //_UTIL_SIMPLE_PROPERTY_SET_H
