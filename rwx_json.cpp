#include "rwx_json.hpp"

JsonValue::JsonValue() : type("null") {}

JsonValue::JsonValue(const string& value) : JsonValue()
{
    type = typeNames.at(type_index(typeid(string)));
    this->value = value;
}

JsonValue::JsonValue(const char* value)
{
    type = typeNames.at(type_index(typeid(string)));
    this->value = string(value, strlen(value));
}

JsonValue::JsonValue(int value) : JsonValue()
{
    type = typeNames.at(type_index(typeid(int)));
    this->value = LexicalCast<string>(value);
}

JsonValue::JsonValue(double value) : JsonValue()
{
    type = typeNames.at(type_index(typeid(double)));
    this->value = LexicalCast<string>(value);
}

JsonValue::JsonValue(bool value) : JsonValue()
{
    type = typeNames.at(type_index(typeid(bool)));
    this->value = LexicalCast<string>(value);
}

JsonValue::JsonValue(std::nullptr_t value) : JsonValue() {}

JsonValue::~JsonValue() {}

inline bool JsonValue::IsInt()
{
    return (type == typeNames.at(type_index(typeid(int))));
}

inline bool JsonValue::IsDouble()
{
    return (type == typeNames.at(type_index(typeid(double))));
}

inline bool JsonValue::IsBool()
{
    return (type == typeNames.at(type_index(typeid(bool))));
}

inline bool JsonValue::IsString()
{
    return (type == typeNames.at(type_index(typeid(string))));
}

inline bool JsonValue::IsNull()
{
    return (type == "null");
}

inline int JsonValue::GetInt()
{
    return LexicalCast<int>(value);
}

inline double JsonValue::GetDouble()
{
    return LexicalCast<double>(value);
}

inline bool JsonValue::GetBool()
{
    return LexicalCast<bool>(value);
}

inline string JsonValue::GetString()
{
    return value;
}

RwxJson::RwxJson() {}

RwxJson::~RwxJson() {}

void RwxJson::ParseJsonFile(const string& fileName)
{
    doc.Clear();
    FILE* fp = fopen(fileName.c_str(), "r");
    if (!fp) {
        cerr << "open file failed. file name: " << fileName;
    } else {
        this->fileName = fileName;
        char readBuffer[READ_BUFFER_SIZE];
        FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        doc.ParseStream<ParseJsonFlags>(is);
        fclose(fp);
    }
}

void RwxJson::ParseJsonStr(const string& str)
{
    doc.Clear();
    StringStream ss(str.c_str());
    doc.ParseStream<ParseJsonFlags>(ss);
}

void RwxJson::ParseJsonContent(const JsonContent& content)
{
    doc.Clear();
    for (auto i = content.begin(); i != content.end(); i++) {
        JsonValue jv = i->second;
        if (jv.IsInt()) {
            Pointer(i->first).Set(doc, jv.GetInt());
        } else if (jv.IsDouble()) {
            Pointer(i->first).Set(doc, jv.GetDouble());
        } else if (jv.IsBool()) {
            Pointer(i->first).Set(doc, jv.GetBool());
        } else if (jv.IsString()) {
            Pointer(i->first).Set(doc, jv.GetString());
        } else if (jv.IsNull()) {
            Pointer(i->first).Create(doc);
        }
    }
}

void RwxJson::Save()
{
    if (fileName.empty()) {
        cerr << "the file name is empty. can't be saved." << endl;
    } else {
        SaveAs(fileName);
    }
}

void RwxJson::SaveAs(const string& fileName)
{
    FILE* fp = fopen(fileName.c_str(), "w");
    if (!fp) {
        cerr << "the file not exist, can't be saved. file name: " << fileName << endl;
    } else {
        char writeBuffer[WRITE_BUFFER_SIZE];
        FileWriteStream fs(fp, writeBuffer, sizeof(writeBuffer));
        Writer<FileWriteStream> writer(fs);
        doc.Accept(writer);
        fclose(fp);
    }
}

JsonContent RwxJson::SaveAsDataStructure()
{
    JsonContent content;
    Convent(content);
    return std::move(content);
}

void RwxJson::Print()
{
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    doc.Accept(writer);
    puts(sb.GetString());
}

bool RwxJson::Erase(const string& path)
{
    bool ret = Pointer(path).Erase(doc);
    return ret;
}

void RwxJson::Convent(JsonContent& content, const string& path)
{
    Value& v = CreateValueByPointer(doc, Pointer(path));
    for (auto& i : v.GetObject()) {
        if (v.IsObject()) {
            Convent(content, path + "/" + i.name.GetString());
        } else if (v.IsArray()) {
            int index = 0;
            for (auto& j : v.GetArray()) {
                Convent(content, path + "/" + to_string(index++));
            }
        } else {
            JsonValue jv;
            if (v.IsString()) {
                jv.Set<string>(v.GetString());
            } else if (v.IsInt()) {
                jv.Set(v.GetInt());
            } else if (v.IsDouble()) {
                jv.Set(v.GetDouble());
            } else if (v.IsBool()) {
                jv.Set(v.GetBool());
            } else if (v.IsNull()) {
                jv.Set("null");
            } else {
                cerr << "not support the unknow type." << endl;
            }
            content[path] = std::move(jv);
        }
    }
}