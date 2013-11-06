#include "StdAfx.h"

// Log.h
#ifdef SIMPLELOG

#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;


/**
 * 用于输出log文件的类.
 */
class Log
{
public:
    Log();
    ~Log();

    bool Open(string strFileName);
    void Close();

    bool CommonLogInit(string directory); //打开默认的log 文件

    void Enable();
    void Disable();

    string GetTimeStr();

    template <typename T> void LogOut(const T& value)
    {
        if (m_bEnabled)
        {
            m_tOLogFile << value;
        }
    }

    template <typename T> void LogOutLn(const T& value)
    {
        if (m_bEnabled)
        {
            m_tOLogFile << value << endl;
        }
    }

    void LogOutLn()
    {
        if (m_bEnabled)
        {
            m_tOLogFile << endl;
        }
    }

    template <typename T> Log& operator<<(const T& value)
    {
        if (m_bEnabled)
        {
            m_tOLogFile << value;
        }
        return (*this);
    }

    Log& operator<<(ostream& (*_Pfn)(ostream&))
    {
        if (m_bEnabled)
        {
            (*_Pfn)(m_tOLogFile);
        }
        return (*this);
    }

private:
    template<typename T> string ValueToStr(T value)
    {
        ostringstream ost;
        ost << value;
        return ost.str();
    }
private:
    ofstream m_tOLogFile;

    bool m_bEnabled;
};


#endif
