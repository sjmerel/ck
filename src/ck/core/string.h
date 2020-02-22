#pragma once

#include "ck/core/platform.h"

namespace Cki
{


class String
{
public:
    String();
    String(int n); // reserves n chars
    String(const char*);
    String(const String&);
//    String(const char*, int n);
//    String(const String&, int n);

    class External {};
    String(char* buf, int bufSize, External); // uses external storage
    String(char* buf, External); // uses external storage

    String& operator=(const String&);
    String& operator=(const char*);

    ~String();

    void printf(const char* format, ...);

    int getLength() const { return m_length; }
    bool isEmpty() const { return m_length == 0; }
    void resize(int len, char fill = ' ');
    void clear();

    // Capacity does not include final null.  When using external
    // storage, capacity may not be increased to the requested amount.
    void reserve(int capacity);
    int getCapacity() const;
    void compact(); // reduce capacity as much as possible

    void assign(const String&);
    void assign(const char*);
//    void assign(const String&, int n);
//    void assign(const char*, int n);

    void append(const String&);
    void append(const char*);
    void append(char);
    void append(const char*, int n);
    void append(const String&, int n);

    const char& operator[](int) const;
    char& operator[](int);

    const char* getBuffer() const;
    char* getBuffer();

    bool equals(const String&, bool ignoreCase = false) const;
    bool equals(const char*, bool ignoreCase = false) const;
    int compare(const String&, bool ignoreCase = false) const;
    int compare(const char*, bool ignoreCase = false) const;

    bool startsWith(const String&, bool ignoreCase = false) const;
    bool startsWith(const char*, bool ignoreCase = false) const;
    bool endsWith(const String&, bool ignoreCase = false) const;
    bool endsWith(const char*, bool ignoreCase = false) const;

    void makeLowerCase();
    void makeUpperCase();

    void erase(int pos, int n = 1);
    void eraseFrom(int pos);
    void eraseTo(int pos);

    void insert(int pos, const String&);
    void insert(int pos, const char*);
    void insert(int pos, char);

    int find(char c) const;
    int find(char c, int start) const;
    int rfind(char c) const;
    int rfind(char c, int start) const;

    bool operator==(const String&) const;
    bool operator==(const char*) const;
    bool operator!=(const String&) const;
    bool operator!=(const char*) const;
    bool operator<(const String&) const;
    bool operator<(const char*) const;
    bool operator>(const String&) const;
    bool operator>(const char*) const;
    bool operator<=(const String&) const;
    bool operator<=(const char*) const;
    bool operator>=(const String&) const;
    bool operator>=(const char*) const;

    static bool equals(const char*, const char*, int n = -1, bool ignoreCase = false);
    static int getLength(const char*);

protected:
    char* m_buf;
    int m_bufSize; // size of buffer
    int m_length; // length of string, not including null; always < bufSize
    bool m_external; // external storage

private:
    static char s_null;
    void appendImpl(const char* str, int strLength);
    void assignImpl(const char* str, int strLength);
    void insertImpl(int pos, const char* str, int strLength);
    bool startsWithImpl(const char* str, int strLength, bool ignoreCase) const;
    bool endsWithImpl(const char* str, int strLength, bool ignoreCase) const;
    int compareImpl(const char* s, bool ignoreCase) const;
};



}
