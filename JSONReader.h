#pragma once

/*
*    Author:  Todd Keitel
*    Date:    20 Aug 2020
*    Title:   Simple JSON Parser
*
*    Scope:   A simple JSON parser.  This code is NOT memory optimized.
*/

#include <string>
#include <fstream>

using namespace std;

class JSONReader
{
public:
  static const int  MAX_JSON = 50;  // This is a limiting factor in this codebase.  Tune it for your data sizes
  static const char OPEN_BRACKET = '{';
  static const char CLOSED_BRACKET = '}';
  static const char COLON = ':';
  static const char COMMA = ',';
  static const char QUOTE = '\"';
  static const char SPACE = ' ';
  static const char UNKNOWN = '~';
  static const char PERIOD = '.';
  static const char HYPEN = '-';
  static const int  NOT_SET = -1;
  static const int  IS_THE_SAME = 0;

  //Simple struct for loading characters into a string
  struct CharReader
  {
  private:
    char m_Num[2];

  public:
    CharReader()
    {
      m_Num[0] = '\0';
      m_Num[1] = '\0';
    }
    virtual ~CharReader() {}

    void Set(char _value) { m_Num[0] = _value; }
    char* GetPtr(void) { return &m_Num[0]; }
    char Get(void) { return m_Num[0]; }
  };

  //A JSONObj is used to hold each individual JSON object.  A JSON message may have any number of JSON objects.   

public:
  enum JSONDataTypes
  {
    eString = 0,
    eInteger,
    eFloat,
    eSize_t,
    eChild,
    eEndOfJSONDataTypes
  };
  class JSONObj;

  class JSONBase
  {
  public:
    string m_Tag;
    JSONDataTypes m_Type;
    JSONBase* m_NextTag;
    JSONBase(JSONDataTypes _type)
      : m_Type(_type)
      , m_NextTag(nullptr)
    {}
    virtual ~JSONBase() {}
  };

  template<typename T>
  class JSONData : public JSONBase
  {
  protected:
    T m_Data;
  public:
    JSONData(JSONDataTypes _type)
      : JSONBase(_type)
    {}

    T GetData(void) { return this->m_Data; }
    T* GetDataPtr(void) { return &(this->m_Data); }
    void SetData(T _data) { this->m_Data = _data; }
  };



  class JSONObj
  {
  private:
    JSONBase* m_Head;
    JSONBase* m_Tail;
    uint32_t m_DataCount;


    void Insert(JSONBase* _data)
    {
      if (nullptr == m_Head)
      {
        m_Head = _data;
      }
      if (nullptr != m_Tail)
      {
        m_Tail->m_NextTag = _data;
      }
      m_Tail = _data;
    }

  public:

    JSONObj()
      : m_Head(nullptr)
      , m_Tail(nullptr)
      , m_DataCount(0)
    {

    }

    virtual ~JSONObj() {}

    void AddData(string _tag, string _value, JSONDataTypes _type)
    {
      JSONData<string>* newtag = new JSONData<string>(_type);
      newtag->m_Tag.assign(_tag.c_str());
      newtag->SetData(_value);
      Insert(newtag);
      ++m_DataCount;
    }

    template<typename T>
    JSONData<T>* AddData(string _tag, T _value, JSONDataTypes _type)
    {
      JSONData<T>* newtag = new JSONData<T>(_type);
      newtag->m_Tag.assign(_tag.c_str());
      newtag->SetData(_value);
      Insert(newtag);
      ++m_DataCount;
      return newtag;
    }

    JSONBase* GetHead(void) { return m_Head; }
    JSONBase* GetTail(void) { return m_Tail; }
    size_t GetTagCount(void) { return m_DataCount; }

    bool GetTagType(size_t _index, JSONDataTypes& _type)
    {
      bool success = false;
      _type = eString;
      if (_index < m_DataCount)
      {
        JSONBase* tag = GetTag(_index);
        if (nullptr != tag)
        {
          success = true;
          _type = tag->m_Type;
        }
      }
      return success;
    }

    JSONBase* GetTag(size_t _index)
    {
      JSONBase* tag = m_Head;
      for (size_t iI = 0; iI < _index; ++iI)
      {
        if (nullptr == tag)
        {
          break;
        }
        tag = tag->m_NextTag;
      }
      return tag;
    }

    bool GetTagSet(size_t _index, string& _tag, int &_data)
    {
      bool success = false;
      if (_index < m_DataCount)
      {
        JSONData<int>* tag = reinterpret_cast<JSONData<int>*>(GetTag(_index));
        if (nullptr != tag)
        {
          _tag.assign(tag->m_Tag.c_str());
          _data = tag->GetData();
          success = true;
        }
      }
      return success;
    }

    bool GetTagSet(size_t _index, string& _tag, string &_data)
    {
      bool success = false;
      if (_index < m_DataCount)
      {
        JSONData<string>* tag = reinterpret_cast<JSONData<string>*>(GetTag(_index));
        if (nullptr != tag)
        {
          _tag.assign(tag->m_Tag.c_str());
          _data.assign(tag->GetData().c_str());
          success = true;
        }
      }
      return success;
    }

    bool GetTagSet(size_t _index, string& _tag, size_t &_data)
    {
      bool success = false;
      if (_index < m_DataCount)
      {
        JSONData<size_t>* tag = reinterpret_cast<JSONData<size_t>*>(GetTag(_index));
        if (nullptr != tag)
        {
          _tag.assign(tag->m_Tag.c_str());
          _data = tag->GetData();
          success = true;
        }
      }
      return success;
    }


    bool GetTagSet(size_t _index, string& _tag, float &_data)
    {
      bool success = false;
      if (_index < m_DataCount)
      {
        JSONData<float>* tag = reinterpret_cast<JSONData<float>*>(GetTag(_index));
        if (nullptr != tag)
        {
          _tag.assign(tag->m_Tag.c_str());
          _data = tag->GetData();
          success = true;
        }
      }
      return success;
    }

    bool GetTagSet(size_t _index, string& _tag, JSONObj** _data)
    {
      bool success = false;
      if (_index < m_DataCount)
      {
        JSONData<JSONObj>* tag = reinterpret_cast<JSONData<JSONObj>*>(GetTag(_index));
        if (nullptr != tag)
        {
          _tag.assign(tag->m_Tag.c_str());
          *_data = &tag->GetData();
          success = true;
        }
      }
      return success;
    }

    JSONObj* GetNextChild(size_t& _index)
    {
      JSONObj* retvalue = nullptr;
      JSONBase* tag = GetTag(_index);
      for (size_t each = _index; each < m_DataCount; ++each)
      {
        if (eChild == tag->m_Type)
        {
          _index = each;
          JSONData<JSONObj>* child = reinterpret_cast<JSONData<JSONObj>*>(tag);
          retvalue = &child->GetData();
          break;
        }
      }
      return retvalue;
    }

    JSONObj* GetChild(size_t _index)
    {
      return reinterpret_cast<JSONData<JSONObj>*>(GetTag(_index))->GetDataPtr();;
    }

    static void Destroy(JSONObj* _obj)
    {
      JSONBase* tag = _obj->m_Head;
      JSONBase* prevtag = _obj->m_Head;
      for (uint32_t iI = 0; iI < _obj->m_DataCount; ++iI)
      {
        if (eChild == tag->m_Type)
        {
          JSONData<JSONObj>* ctag = reinterpret_cast<JSONData<JSONObj>*>(tag);
          ctag->GetData().Destroy(&ctag->GetData());
          prevtag->m_NextTag = ctag->m_NextTag;
          delete ctag;
          tag = prevtag->m_NextTag;
        }
        else
        {
          JSONBase* temp = prevtag->m_NextTag;
          prevtag = tag;
          tag = temp;
          if (nullptr == tag)
          {
            break;
          }
        }
      }
    }

  };

private:
  JSONObj m_JSONObj;
  string m_Buffer;
  size_t m_Index;

public:

  JSONReader()
    : m_Index(0)
  {
  }

  JSONReader(ifstream* _file)
    : m_Index(0)
  {
    Read(_file);
  }

  virtual ~JSONReader()
  {
    m_JSONObj.Destroy(&m_JSONObj);
  }

  void Shutdown(void)
  {
    m_JSONObj.Destroy(&m_JSONObj);
  }

  void Read(ifstream* _file)
  {
    m_Index = 0;
    m_Buffer.clear();
    if (_file->is_open())
    {
      string temp;
      while (!_file->eof())
      {
        getline(*_file, temp);
        m_Buffer.append(temp.c_str());
      }
      _file->close();

      Parse(&m_Buffer, &m_JSONObj);
    }
  }

  JSONDataTypes GetType(JSONObj* _obj, string* _tag, size_t& _index)
  {
    JSONBase* tag = FindTag(_obj, _tag, _index);
    return tag->m_Type;
  }

  JSONBase* FindTag(JSONObj* _obj, string* _tag, size_t& _index)
  {
    JSONBase* tag = nullptr;
    if (nullptr != _obj)
    {
      tag = _obj->GetHead();
      size_t count = _obj->GetTagCount();
      for (size_t iI = 0; iI < count; ++iI)
      {
        if (nullptr == tag)
        {
          break;
        }
        if (IS_THE_SAME == tag->m_Tag.compare(_tag->c_str()))
        {
          _index = iI;
          break;
        }
        else
        {
          tag = tag->m_NextTag;
        }
      }
    }
    return tag;
  }

  size_t GetDataCount(JSONObj* _obj = nullptr)
  {
    JSONObj* obj = nullptr != _obj ? _obj : &m_JSONObj;
    size_t datacount = 0;
    if (nullptr != obj)
    {
      datacount = obj->GetTagCount();
    }
    return datacount;
  }

  bool GetNextTagType(JSONObj* _obj, size_t& _index, JSONDataTypes& _type)
  {
    bool success = false;
    _type = eString;
    JSONBase* tag = _obj->GetTag(_index++);
    if (nullptr != tag)
    {
      _type = tag->m_Type;
      success = true;
    }
    return success;
  }

  template<typename T>
  T GetTag(JSONObj* _obj, string* _tag, size_t _index)
  {
    bool success = false;
    T retvalue;
    if (_index < _obj->GetTagCount())
    {
      success = _obj->GetTagSet(_tag, _index, &retvalue);
    }
    return retvalue;
  }

  JSONObj* GetAncestor(void) { return &m_JSONObj; }

private:

  void Parse(string* _buffer, JSONObj* _obj)
  {
    size_t index = 0;
    bool done = false;
    while (!done && (index < _buffer->length()))
    {
      done = !GetNextPair(_buffer, index, _obj);
    }
  }


  bool IsValidChar(char _value)
  {
    bool success = true;
    success &= !((QUOTE == _value) || (COLON == _value) || (COMMA == _value) || (OPEN_BRACKET == _value) || (CLOSED_BRACKET == _value) || SPACE == _value);
    return success;
  }

  bool IsAlphaNumeric(char _value)
  {
    return ('a' <= _value && _value <= 'z') || ('A' <= _value && _value <= 'Z') || ('0' <= _value && _value <= '9') || (HYPEN == _value) || (PERIOD == _value);
  }


  bool FindNextChar(string* _buffer, size_t& _index, char _value)
  {
    bool success = false;
    while (_index < _buffer->length())
    {
      if (_value == _buffer->at(_index++))
      {
        success = true;
        break;
      }
    }
    return success;
  }

  bool GetNextPair(string* _buffer, size_t& _index, JSONObj* _obj)
  {
    bool success = false;
    string tag("");
    if (GetNextTag(_buffer, _index, tag))
    {
      char firstdatachar = FindStartOfData(_buffer, _index);
      switch (firstdatachar)
      {
      case QUOTE:
      {
        string data = ReadToQuote(_buffer, _index);
        _obj->AddData(tag, data, eString);
        success = true;
        break;
      }
      default:
      {
        string data = ReadToCommaOrBracket(_buffer, _index);
        if (FoundDecimalPoint(data))
        {
          float fdata = stof(data);
          _obj->AddData(tag, fdata, eFloat);
        }
        else if (FoundNegativeSign(data))
        {
          int idata = stoi(data);
          _obj->AddData(tag, idata, eInteger);
        }
        else
        {
          if (IsANumber(&data))
          {
            size_t sdata = static_cast<size_t>(stoi(data));
            _obj->AddData(tag, sdata, eSize_t);
          }
          else
          {
            success = false;
          }
        }
        success = true;
        break;
      }
      case OPEN_BRACKET:
      {
        JSONObj* child = new JSONObj();
        JSONData<JSONObj>* newtag = _obj->AddData(tag, *child, eChild);
        size_t childindex = 0;
        _index++;
        string* newdata = new string(ReadToClosingBracket(_buffer, _index).c_str());
        Parse(newdata, newtag->GetDataPtr());
        success = true;
        break;
      }
      }
    }
    return success;
  }

  string ReadToQuote(string* _buffer, size_t& _index)
  {
    string retvalue("");
    CharReader cr;
    bool done = false;
    while (!done && _index < _buffer->length())
    {
      cr.Set(_buffer->at(_index++));
      if (QUOTE == cr.Get())
      {
        done = true;
      }
      else
      {
        retvalue.append(cr.GetPtr());
      }
    }
    return retvalue;
  }

  string ReadToCommaOrBracket(string* _buffer, size_t& _index)
  {
    string retvalue("");
    CharReader cr;
    bool done = false;
    while (!done && _index < _buffer->length())
    {
      cr.Set(_buffer->at(_index++));
      if ((COMMA == cr.Get()) || (CLOSED_BRACKET == cr.Get()) || (SPACE == cr.Get()))
      {
        done = true;
      }
      else
      {
        retvalue.append(cr.GetPtr());
      }
    }
    return retvalue;
  }

  string ReadToClosingBracket(string* _buffer, size_t& _index)
  {
    string retvalue("");
    size_t openbracketcount = 0;
    bool done = false;
    CharReader cr;
    while (!done && _index < _buffer->length())
    {
      cr.Set(_buffer->at(_index++));
      if (CLOSED_BRACKET == cr.Get())
      {
        if (0 == openbracketcount)
        {
          done = true;
        }
        else
        {
          --openbracketcount;
          retvalue.append(cr.GetPtr());
          done = true;
          break;
        }
      }
      else if (OPEN_BRACKET == cr.Get())
      {
        ++openbracketcount;
        retvalue.append(cr.GetPtr());
      }
      else
      {
        retvalue.append(cr.GetPtr());
      }
    }
    return retvalue;
  }

  bool FoundDecimalPoint(string _data)
  {
    bool founddecimal = false;
    for (size_t iI = 0; iI < _data.length(); ++iI)
    {
      if (PERIOD == _data.at(iI))
      {
        founddecimal = true;
        break;
      }
    }
    return founddecimal;
  }

  bool FoundNegativeSign(string _data)
  {
    return HYPEN == _data.at(0);
  }

  char FindStartOfData(string* _buffer, size_t& _index)
  {
    char retvalue = UNKNOWN;
    bool done = false;
    while (!done && _index < _buffer->length())
    {
      retvalue = (_buffer->at(_index));
      bool alpha = IsAlphaNumeric(retvalue);
      bool bracket = OPEN_BRACKET == retvalue;
      done = (QUOTE == retvalue) || alpha || bracket;
      _index += !done ? 1 : alpha ? 0 : 1;
    }
    return retvalue;
  }

  string GetRawData(string* _buffer, size_t& _index)
  {
    string retvalue("");
    bool done = false;
    bool started = false;
    size_t openBracketCount = 0;
    bool inQuotes = false;
    CharReader cr;
    while (!done && _index < _buffer->length())
    {
      cr.Set(_buffer->at(_index++));
      if (!started)
      {
        if (COLON == cr.Get())
        {
          started = true;
        }
      }
      else
      {
        switch (cr.Get())
        {
        default:
        {
          retvalue.append(cr.GetPtr());
          break;
        }
        case CLOSED_BRACKET:
        {
          if (0 < openBracketCount)
          {
            retvalue.append(cr.GetPtr());
            --openBracketCount;
          }
          if (1 >= openBracketCount)
          {
            done = true;
          }
          break;
        }
        case OPEN_BRACKET:
        {
          ++openBracketCount;
          retvalue.append(cr.GetPtr());
          break;
        }
        case QUOTE:
        {
          inQuotes = !inQuotes ? true : false;
          retvalue.append(cr.GetPtr());
          break;
        }
        case COMMA:
        {
          if (inQuotes || (0 < openBracketCount))
          {
            retvalue.append(cr.GetPtr());
          }
          else
          {
            done = true;
          }
          break;
        }
        }
      }
    }
    return retvalue;
  }

  bool GetNextTag(string* _buffer, size_t& _index, string& _tag)
  {
    _tag.clear();
    CharReader cr;
    if (FindNextChar(_buffer, _index, QUOTE))
    {
      cr.Set(_buffer->at(_index++));
      while ((QUOTE != cr.Get()) && (_index < _buffer->length()))
      {
        _tag.append(cr.GetPtr());
        cr.Set(_buffer->at(_index++));
      }
    }
    return 0 < _tag.length();
  }

  bool GetNextValueString(string* _buffer, size_t& _index, string& _value)
  {
    _value.clear();
    CharReader cr;
    bool found = false;
    while (_index < _buffer->length() && !found)
    {
      cr.Set(_buffer->at(_index++));
      if (IsValidChar(cr.Get()))
      {
        found = true;
        _value.append(cr.GetPtr());

      }
      else if (found)
      {
        break;
      }
    }
    return found;
  }

  static bool IsANumber(string* _word, bool _allowk = false)
  {
    size_t digitcount = _word->length();
    if ('-' == _word->at(0))
    {
      digitcount -= 1;
    }
    for (size_t iI = 0; iI < _word->length(); ++iI)
    {
      digitcount -= (_word->at(iI) >= '0') && (_word->at(iI) <= '9') ? 1 : 0;
      if ((',' == _word->at(iI) || '.' == _word->at(iI)) && (digitcount < _word->length()))
      {
        digitcount -= 1;
      }
      if (_allowk)
      {
        if ('k' == _word->at(iI))
        {
          digitcount -= 1;
        }
      }
    }
    return digitcount == 0;
  }
};