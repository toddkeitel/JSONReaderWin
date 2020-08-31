#include "JSONReader.h"
#include <stdint.h>
#include <string>
#include <cctype>
#include <algorithm>
#include <time.h>
#include <iostream>
#include <fstream>

class Saver
{
public:
  static const int MAXCOUNTER = 100;
private:
  Saver()
  : m_Counter(0)
  {}
  JSONReader::JSONObj* m_Saver[100];
  int m_Counter;
public:
  static Saver* GetInstance(void)
  {
    static Saver* m_Saver = nullptr;
    if (nullptr == m_Saver)
    {
      m_Saver = new Saver();
    }
    return m_Saver;
  }

  int Push(JSONReader::JSONObj* _child)
  {
    int retvalue = -1;
    if (m_Counter >= MAXCOUNTER)
    {
      m_Counter = 0;
    }
    retvalue = m_Counter++;
    m_Saver[retvalue] = _child;
    return retvalue;
  }

  JSONReader::JSONObj* Pop(int _index)
  {
    JSONReader::JSONObj* retvalue = nullptr;
    if (0 <= _index && _index < MAXCOUNTER)
    {
      retvalue = m_Saver[_index];
    }
    return retvalue;
  }

};
JSONReader::JSONObj* m_Saver[100];

static void WriteChild(int _index)
{
  JSONReader::JSONObj* _obj = Saver::GetInstance()->Pop(_index);
  //if (nullptr != _obj)
  //{
    size_t count = _obj->GetTagCount();
    string tag("");
    int ivalue = 0;
    float fvalue = float(0);
    string svalue("");
    size_t tvalue = size_t(0);
    JSONReader::JSONDataTypes type = JSONReader::eString;
    for (size_t each = 0; each < count; ++each)
    {
      if (_obj->GetTagType(each, type))
      {
        switch (type)
        {
        case JSONReader::eString:
        default:
        {
          if (_obj->GetTagSet(each, tag, svalue))
          {
            std::cout << tag.c_str() << " : " << svalue.c_str() << std::endl;
          }
          break;
        }
        case JSONReader::eInteger:
        {
          if (_obj->GetTagSet(each, tag, ivalue))
          {
            std::cout << tag.c_str() << " : " << ivalue << std::endl;
          }
          break;
        }
        case JSONReader::eSize_t:
        {
          if (_obj->GetTagSet(each, tag, tvalue))
          {
            std::cout << tag.c_str() << " : " << tvalue << std::endl;
          }
          break;
        }
        case JSONReader::eFloat:
        {
          if (_obj->GetTagSet(each, tag, fvalue))
          {
            std::cout << tag.c_str() << " : " << fvalue << std::endl;
          }
          break;
        }
        case JSONReader::eChild:
        {
           int index = Saver::GetInstance()->Push(_obj->GetChild(each));
           WriteChild(index);
           break;
        }
        }
      }
    }
  //}
}

int main(int argc, char* argv[])
{
  string filename("c:/Users/Public/textblast_1.json");
  ifstream* filein = new ifstream(filename.c_str(), ios::in);
  JSONReader* reader = nullptr;
  if (filein->is_open())
  {
    reader = new JSONReader(filein);
  }
  if (nullptr != reader)
  {
    size_t data_count = reader->GetDataCount();
    JSONReader::JSONObj* parent = reader->GetAncestor();
    if (nullptr != parent)
    {
      std::cout << "Parent object found with reported member count " << data_count << std::endl;
      std::cout << std::endl;
      int index = Saver::GetInstance()->Push(parent);
      WriteChild(index);
    }
  }

  return 0;
}