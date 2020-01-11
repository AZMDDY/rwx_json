#ifndef RWX_JSON_HPP
#define RWX_JSON_HPP
#include <iostream>
#include <regex>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#define RAPIDJSON_HAS_STDSTRING 1  // 支持std::string
#include "lexical_cast.hpp"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/pointer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"

using std::cerr;
using std::endl;
using std::pair;
using std::regex;
using std::regex_match;
using std::sregex_token_iterator;
using std::string;
using std::to_string;
using std::type_index;
using std::unordered_map;
using namespace rapidjson;

// 放宽的json语法
#define ParseJsonFlags (kParseCommentsFlag | kParseTrailingCommasFlag | kParseNanAndInfFlag)

#define READ_BUFFER_SIZE 1024 * 64   // 64 KB
#define WRITE_BUFFER_SIZE 1024 * 64  // 64 KB

// 类型-值映射
const unordered_map<type_index, string> typeNames = {{type_index(typeid(string)), "string"},
                                                     {type_index(typeid(int)), "int"},
                                                     {type_index(typeid(double)), "double"},
                                                     {type_index(typeid(bool)), "bool"}};

// 存放json值的类(前置声明)
class JsonValue;

// 保存json内容的数据结构: <path, value>
using JsonContent = unordered_map<string, JsonValue>;

/** Rwxjson
 * 接口说明：
 * 1. 读： 可以从文件中、字符串、JsonContent数据结构中读取json信息;
 * 2. 写： 可以将内存中的json信息保存为文件或JsonContent数据结构中;
 * 3. 操作： 对内存的json信息进行Set、Get、Erase操作;
 * 注意：
 *  1. 对于RwxJson的一个对象，读会清空之前读入内存的json信息；
 *  2. 对json内容的Set、Get、Erase操作，都是以字符串路径作为key，格式为 "/XXX/XXX"
 */

class RwxJson {
public:
    RwxJson();
    ~RwxJson();
    void ParseJsonFile(const string& fileName);         // 解析json文件
    void ParseJsonStr(const string& str);               // 解析json字符串
    void ParseJsonContent(const JsonContent& content);  // 解析JsonConent

    void Save();
    void SaveAs(const string& fileName);
    JsonContent SaveAsDataStructure();  // 保存为特定的数据结构(JsonContent)

    void Print();

    /** 创建/修改json内容
     * @ param: path: json路径。(key的路径不完整，会自动创建key的父值)
     */
    template <typename T>
    bool Set(const string& path, const T& val, bool numIsKey = false)
    {
        auto type = type_index(typeid(T));
        if (typeNames.find(type) != typeNames.end()) {
            regex rule("^/[[:print:]]*(?!/)$");
            bool ret = regex_match(path, rule);
            if (ret) {
                if (!numIsKey) {
                    Pointer(path).Set(doc, val);
                } else {
                    regex rule("/\\d+");
                    const sregex_token_iterator end;
                    for (sregex_token_iterator i(path.begin(), path.end(), rule); i != end; i++) {
                        const string subStr = path.substr(0, path.find(*i));
                        auto& a = doc.GetAllocator();
                        SetValueByPointer(doc, Pointer(subStr + "/"), "", a);
                        Value& v = CreateValueByPointer(doc, Pointer(subStr));
                        v.RemoveMember("");
                    }
                    SetValueByPointer(doc, Pointer(path), val);
                }
            } else {
                cerr << "the path is incorrent. " << endl;
                return false;
            }
        } else {
            cerr << "the type is not support. type: " << type.name() << endl;
            return false;
        }
    }

    bool Erase(const string& path);

    template <typename T>
    T Get(const string& path, T& val = {})
    {
        auto type = type_index(typeid(T));
        if (typeNames.find(type) != typeNames.end()) {
            Value* v = nullptr;
            v = Pointer(path).Get(doc);
            if (v != nullptr) {
                val = v->Get<T>();
            } else {
                cerr << "the path not exist. path: " << path << endl;
            }
            return val;
        } else {
            cerr << "the type is not support. type: " << type.name() << endl;
            return {};
        }
    }

private:
    // 将json文件中指定路径中的键值存入JsonContent结构
    // 注意： 此函数为内部递归调用，为了将整个json内容都转化成JsonContent结构，
    // path参数的值只能是空！！！不能是 "/"。
    // 如果需要只需要转化json内容中的一部分，那么path格式还是 "/xxx/xx/xx"。
    void Convent(JsonContent& content, const string& path = "");

private:
    Document doc;
    string fileName;
};

/** Json值类
 * 可以隐式构造JsonValue对象，更为直观；
 * 提供Json值的类型判断，并提供获取值的接口;
 * 在类中Json的值被保存为string类型，但可以通过相应接口获取实际类型的值;
 * 目前支持json值的类型: int, double, bool, string, null;
 */
class JsonValue {
public:
    JsonValue();
    JsonValue(const string& value);
    JsonValue(const char* value);
    JsonValue(int value);
    JsonValue(double value);
    JsonValue(bool value);
    JsonValue(std::nullptr_t value);
    ~JsonValue();

    bool IsInt();
    bool IsDouble();
    bool IsBool();
    bool IsString();
    bool IsNull();

    int GetInt();
    double GetDouble();
    bool GetBool();
    string GetString();

    template <typename T>
    bool Set(const T& val)
    {
        auto type = type_index(typeid(T));
        if (typeNames.find(type) != typeNames.end()) {
            this->type = typeNames.at(type);
            value = LexicalCast<string>(val);
            return true;
        } else {
            cerr << "the type is not support. type: " << type.name() << endl;
            return false;
        }
    }

private:
    string type;
    string value;
};

#endif  // RWX_JSON_HPP