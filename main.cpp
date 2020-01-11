#include <iostream>
#include <string>
#include "rwx_json.hpp"

using namespace std;
int main(int argc, char** argv)
{
    RwxJson rwxjson;
    JsonContent jc;
    jc["/name"] = "zzb";
    jc["/age"] = 24;
    jc["/friend/0/name"] = "hmt";
    jc["/friend/0/age"] = 22;
    jc["/friend/1/name"] = "hzw";
    jc["/friend/1/age"] = 22;
    jc["/boy"] = true;
    jc["/girlfriend"] = nullptr;
    rwxjson.ParseJsonContent(jc);
    rwxjson.Print();
    return 0;
}