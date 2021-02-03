//
// Created by 32119 on 2021/2/1.
//

/*

    {
        "name": "Jack",
        "format":
        {
            "type": "rect",
            "width": 1920,
            "height": 1080,
            "interlace": false,
            "frame rate": 24
        }
    }

    逻辑上的树状结构
    root -> chind(name)
                  format -> child(type)
                                  width
                                  height
                                  interlace
                                  frame rate

*/

#ifndef INC_2_1_C_JSON_H
#define INC_2_1_C_JSON_H

#define cJSON_False 0; // bool 假
#define cJSON_True 1; // bool 真
#define cJSON_NULL 2; // 空
#define cJSON_Number 3; // 整数类型
#define cJSON_String 4; // 字符串
#define cJSON_Array 5; // 数组
#define cJSON_Object 6; // 对象

// 功能：创建一个string值为name的cJSON_False节点，并添加到object节点
#define cJSON_AddFalseToObject(object, name) cJSON_AddItemToObject(object, name, cJSON_CreateFalse())

// 功能：创建一个string值为name的cJSON_True节点，并添加到object节点
#define cJSON_AddTrueToObject(object, name) cJSON_AddItemToObject(object, name, cJSON_CreateTrue())

// 功能：创建一个string值为name的cJSON_Bool/False节点，并添加到object节点
#define cJSON_AddBoolToObject(object, name, b) cJSON_AddItemToObject(object, name, cJSON_CreateBool(b))

// 功能：创建一个string值为name的cJSON_Null节点，并添加到object节点
#define cJSON_AddNULLToObject(object, name) cJSON_AddItemToObject(object, name, cJSON_CreateNull())

// 功能：创建一个string值为name的cJSON_False节点，并添加到object节点
#define cJSON_AddNumberToObject(object, name, number) cJSON_AddItemToObject(object, name, cJSON_CreateNumber(number))

// 功能：创建一个string值为name的cJSON_String节点，并添加到object节点
#define cJSON_AddStringToObject(object, name, s) cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

#define cJSON_SetIntValue(object, val) ((object) ? ((object) -> valueInt = (object) -> valueDouble = (val)):(val))

#define cJSON_SetNumberValue(object, val) ((object) ? ((object) -> valueInt = (object) -> valueDouble = (val)):(val))


typedef struct cJSON
{
    struct cJSON * next, * prev; // 双向链表指针域
    struct cJSON * child; // 指向儿子结点的指针域

    int type; // Value(值)的类型

    char * valueString; // 装字符串的数据域
    int valueInt; // 装整形的数据域
    double valueDoudle; // 装浮点型的数据域

    char * string; // 装对象的数据域
}cJSON;

// 常用解析函数

/*
解析函数功能
    解析json数据，将数据填入json树中
参数
    value (const char *):char *指针，指向待解析的json数据
返回值
    json树的根节点
*/
cJSON * cJSON_Parse(const char * value);

/*
打印函数功能
    从item节点开始递归遍历，将节点树转换为字符串
参数
    item(cJDON * item):cJson节点
返回值
    字符串指针，从item节点解析后的树字符串
注意事项
    使用该函数后，需要根据返回值的char *指针释放内存
*/
char * cJSON_Print(cJSON * item);

/*
删除函数功能
    从根节点开始递归删除json树各个节点，释放内存
参数
    c(cJson * c):cJson节点
返回值
    void

*/
void cJSON_Delete(cJSON * c);

/*
创造节点函数功能
    创造一个cJson节点，并设置节点类型为cJson_Object
参数
    void
返回值
    cJson_Object 类型的节点指针
*/
cJSON * cJSON_CreateObject();

/*
函数功能
    创建一个cJSON节点，并设置节点类型为cJSON_String
参数
    string(char * string)
返回值
    cJSON_String 类型的节点指针
*/
cJSON * cJSON_CreateString(const char * string);

/*
函数功能
    创建一个cJSON节点，并设置节点类型为cJSON_Number
参数
    num(doudle)
返回值
    cJSON_Number 类型的节点指针
*/
cJSON * cJSON_CreateNumber(double num);

/*
函数功能
    创建一个cJSON节点，并设置节点类型为cJSON_Array
参数
    void
返回值
    cJSON_Array 类型的节点指针
*/
cJSON * cJSON_CreateArray();

/*
函数功能
    创建一个cJSON节点，并设置节点类型为cJSON_Bool
参数
    b(int)
返回值
    cJSON_Bool 类型的节点指针
*/
cJSON * cJSON_CreateBool(int b);

/*
函数功能
    创建一个cJSON节点，并设置节点类型为cJSON_True
参数
    void
返回值
    cJSON_True 类型的节点指针
*/
cJSON * cJSON_CreateTrue();

/*
函数功能
    创建一个cJSON节点，并设置节点类型为cJSON_False
参数
    void
返回值
    cJSON_False 类型的节点指针
*/
cJSON * cJSON_CreateFalse();

/*
函数功能
    创建一个cJSON节点，并设置节点类型为cJSON_Null
参数

返回值
    cJSON_Null 类型的节点指针
*/
cJSON * cJSON_CreateNull();

/*
函数功能
    将item节点名称设置为string
    如果object节点没有子节点，就将item设置为object节点的子节点
    否则将item添加到object->child链表尾部，成为object子节点的兄弟节点
参数
    object(cJSON *):被添加节点
    string(char *):要添加的节点的名称
    item(cJSON *):要添加的节点
返回值
    void
*/
void cJSON_AddItemToObject(cJSON * object, const char * string, cJSON * item);

/*
函数功能
    将item添加到array节点
参数
    array(cJSON *):被添加节点
    item(cJSON *):要添加的节点
返回值
    void
*/
void cJSON_AddItemToArray(cJSON * array, cJSON * item);

cJSON * cJSON_ParseWithOpts(const char * value, const char ** return_parse_end, int require_null_terminated);

cJSON * cJSON_New_Item();

const char * skip(const char * in);
char * cJSON_GetEndPosition();

const char * parse_value(cJSON * item, const char * value);

const char *parse_string(cJSON * item, const char * str);
const char *parse_number(cJSON * item, const char * num);
const char *parse_Array(cJSON * item, const char * value);
const char *parse_Object(cJSON * item, const char * value);

char * cJSON_strdup(const char * str);
char * print_number(cJSON * item);
char * print_string(cJSON * item, int isName);
char * print_array(cJSON * item, int depth, int fmt);
char * print_object(cJSON * item, int depth, int fmt);
char * print_value(cJSON * item, int depth, int fmt);

cJSON * cJSON_DetachItemFromArray(cJSON * array, int which);
void cJSON_DeleteItemFromArray(cJSON * array, int which);

int cJSON_strcasecmp(const char * s1, const char * s2);

cJSON * cJSON_DetachItemFromObject(cJSON * object, const char * string);
void cJSON_DeleteItemFromObject(cJSON * object, int which);

void cJSON_InsertItemInArray(cJSON * array, int which, cJSON * newItem);

void cJSON_ReplaceItemInArray(cJSON * array, int which, cJSON * newItem);
void cJSON_ReplaceItemInObject(cJSON * object, const char * string, cJSON * newItem);

#endif //INC_2_1_C_JSON_H



















