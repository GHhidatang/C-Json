#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>

#include "c_json.h"

cJSON * cJSON_ParseWithOpts(const char * value, const char ** return_parse_end, int require_null_terminated);

int main()
{
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
    */

    return 0;
}

static void * (* cJSON_malloc)(size_t sz) = malloc;
static void (* cJSON_free)(void * ptr) = free;

static const char * ep;

cJSON * cJSON_New_Item()
{
    cJSON * node = (cJSON *)cJSON_malloc(sizeof(cJSON));
    if (node)
    {
        memset(node, 0, sizeof(cJSON));
    }

    return node;
}

void cJSON_Delete(cJSON * c)
{
    cJSON * next;
    while (c)
    {
        next = c -> next;
        if (c -> child)
            cJSON_Delete(c -> child);
        if (c -> valueString)
            cJSON_free(c -> valueString);
        if (c -> string)
            cJSON_free(c -> string);
        cJSON_free(c);
        c = next;
    }
}

const char * skip(const char * in)
{
    while (in && *in && (unsigned char) *in <= 32)
    {
        in++;
    }

    return in;
}


const char * parse_string(cJSON * item, const char * str)
{
    const char * ptr = str + 1;
    char * ptr2;
    char * out;
    int len = 0;

    if (* str != '\"')
    {
        ep = str;
        return NULL;
    }

    while (* ptr != '\"' && *ptr & ++len)
    {
        if (* ptr++ == '\\')
            ptr++;
    }

    out = (char *)cJSON_malloc(len + 1);
    if (!out)
        return NULL;

    ptr = str + 1;
    ptr2 = out;

    while (* ptr != '"' && * ptr)
    {
        if (* ptr != '\\')
            *ptr2++ = *ptr++;
    }
    *ptr2 = 0;

    if (*ptr == '"')
        ptr++;

    item -> valueString = out;
    item -> type = cJSON_String;

    return ptr;
}

const char * parse_number(cJSON * item, const char * num)
{
    double n = 0;
    int sign = 1;
    int signSubScale = 1;
    int scale = 0;
    int subscale = 0;

    if (*num == '-')
    {
        sign = -1;
        num++;
    }

    if (*num == '0')
        num++;
    if (*num >= '0' && *num <= '9')
    {
        do
        {
            n = (n * 10.0) + (*num++ - '0');

        } while (*num >= '0' && *num <= '9');
    }

    if (*num == '.' && num[1] >= '0' && num[1] <= '9')
    {
        num++;
        do
        {
            n = (n * 10.0) + (*num++ - '0');
        } while (*num >= '0' && *num <= '9');
    }

    if (*num == 'e' || *num == 'E')
    {
        num++;
        if (*num == '+')
            num++;
        else if (*num == '-')
        {
            signSubScale = -1;
            num++;
        }
        do
        {
            subscale = (subscale * 10.0) + (*num++ - '0');
            scale--;
        } while (*num >= '0' && *num <= '9');
    }

    n = sign * n * pow(10.0, (scale + signSubScale*subscale));

    item -> valueDoudle = n;
    item -> valueInt = (int) n;
    item -> type = cJSON_Number;

    return num;
}

const char * parse_Array(cJSON * item, const char * value)
{
    cJSON * child;
    if (* value != '[')
    {
        ep = value;
        return NULL;
    }

    item -> type = cJSON_Array;
    value = skip(value + 1);
    if (*value == ']')
        return value + 1;

    item -> child = child = cJSON_New_Item();
    if (! item -> child)
        return NULL;

    value = skip(parse_value(child, skip(value)));
    if (! value)
        return NULL;

    while (*value == ',')
    {
        cJSON * new_item;
        if (!(new_item = cJSON_New_Item()))
            return NULL;

        child -> next = new_item;
        new_item -> prev = child;
        child = new_item;

        value = skip(parse_value(child, skip(value + 1)));
        if (! value)
            return NULL;
    }

    if (*value == ']')
        return value + 1;
    ep = value;

    return NULL;
}

const char * parse_Object(cJSON * item, const char * value)
{
    cJSON * child;
    if (* value != '{')
    {
        ep = value;
        return NULL;
    }

    item -> type = cJSON_Object;
    value = skip(value + 1);
    if (*value == ']')
        return value + 1;

    item -> child = child = cJSON_New_Item();
    if (! item -> child)
        return NULL;

    value = skip(parse_string(child, skip(value)));
    if (! value)
        return NULL;

    child -> string = child -> valueString;
    child -> valueString = NULL;
    if (*value != ':')
    {
        ep = value;
        return NULL;
    }

    value = skip(parse_value(child, skip(value + 1)));
    if (! value)
        return NULL;

    while (*value == ',')
    {
        cJSON * new_item;
        if (!(new_item = cJSON_New_Item()))
            return NULL;

        child -> next = new_item;
        new_item -> prev = child;
        child = new_item;

        value = skip(parse_string(child, skip(value + 1)));
        if (! value)
            return NULL;

        child -> string = child -> valueString;
        child -> valueString =NULL;

        if (*value != ':')
        {
            ep = value;
            return NULL;
        }

        value = skip(parse_value(child, skip(value + 1)));
        if (! value)
            return NULL;
    }

    if (*value == '}')
        return value + 1;
    ep = value;

    return NULL;
}


const char * parse_value(cJSON * item, const char * value)
{
    if (!value)
        return NULL;
    if (!strncmp(value, "false", 5))
    {
        item -> type = cJSON_False;
        return value + 5;
    }

    if (!strncmp(value, "true", 4))
    {
        item -> type = cJSON_True;
        item -> valueInt = 1;
        return value + 4;
    }

    if (!strncmp(value, "null", 4))
    {
        item -> type = cJSON_NULL;
        return value + 4;
    }

    if (* value == '\"')
        return parse_string(item, value);
    if (* value == '-' || (* value >= '0' && * value <= '9'))
        return parse_number(item, value);
    if (* value == '[')
        return parse_Array(item, value);
    if (* value == '{')
        return parse_Object(item, value);

    ep = value;
    return NULL;
}

cJSON * cJSON_Parse(const char * value)
{
    return cJSON_ParseWithOpts(value, 0, 0);
}

cJSON * cJSON_ParseWithOpts(const char * value, const char ** return_parse_end, int require_null_terminated)
{
    const char * end = NULL;
    cJSON * c = cJSON_New_Item();

    ep = NULL;
    if (!c)
        return NULL;

    end = parse_value(c, skip(value));
    if (!end)
    {
        cJSON_Delete(c);
        return NULL;
    }

    if (require_null_terminated)
    {
        end = skip(end);
        if (* end)
        {
            cJSON_Delete(c);
            ep = end;
            return NULL;
        }
    }

    if (return_parse_end)
    {
        * return_parse_end = end;
    }

    return 0;
}

/*============================================================================================================*/

char * cJSON_strdup(const char * str)
{
    char * copy;
    size_t len;

    len = strlen(str);
    copy = (char *) cJSON_malloc(len + 1);
    if (! copy)
        return NULL;
    memcpy(copy, str,  len + 1);

    return copy;

}

char * print_number(cJSON * item)
{
    char * str = NULL;
    double d = item -> valueDoudle;
    if (d == 0)
    {
        str = (char *) cJSON_malloc(2);
        if (str)
            strcpy(str, '0');
    }else if((fabs((double)(item -> valueInt) - d) <= DBL_EPSILON) && d <= INT_MAX && d >= INT_MIN)
    {
        str = (char *) cJSON_malloc(21);
        if (str)
            sprintf(str, "%d", item -> valueInt);
    } else
    {
        str = (char *) cJSON_malloc(64);
        if (str)
        {
            if ((fabs(floor(d) - d) <= DBL_EPSILON) && fabs(d) < 1.0e60)
            {
                sprintf(str, "%.0f", d);
            } else if (fabs(d) < 1.0e-6 || fabs(1.0e9))
            {
                sprintf(str, "%e", d);
            } else
            {
                sprintf(str, "%i", d);
            }
        }
    }

    return str;
}

char * print_string(cJSON * item)
{
    char * str = item -> valueString;
    char * ptr, * out, *ptr2;
    unsigned char token;
    int flag = 0, len = 0;

    for (ptr = str; *ptr; ptr++)
    {
        flag |= ((*ptr > 0 && *ptr < 32) || (*ptr == '\"') || (*ptr == '\\')) ? 1 : 0;
    }
    if (! flag)
    {
        len = ptr - str;
        out = (char *) cJSON_malloc(len + 3);
        if (! out)
            return NULL;

        ptr2 = out;
        *out++ = '\"';
        strcpy(ptr2, str);
        ptr[len] = '\"';
        ptr2[len] = '\0';

        return out;
    }
    if (! str)
    {
        out = (char *) cJSON_malloc(3);
        if (! out)
            return NULL;
        strcpy(out, "\"\"");
        return out;
    }

    ptr = str;
    while ((token = *ptr) && ++len)
    {
        if (strchr("\"\\\b\f\n\r\t", token))
            len++;
        else if(token < 32)
        {
            len += 5;
            ptr++;
        }
    }

    out = (char *) cJSON_malloc(len + 3);
    if (! out)
        return NULL;

    ptr2 = out;
    ptr = str;
    *ptr2 = '\"';

    while (*ptr)
    {
        if ((unsigned char) *ptr > 31 && *ptr != '\"' && *ptr != '\\')
        {
            *ptr2 = *ptr++;
        } else
        {
            *ptr2++ = '\\';
            switch (token = *ptr)
            {
                case '\\':
                    *ptr2++ = '\\';
                    break;
                case '\"':
                    *ptr2++ = '\"';
                    break;
                case '\b':
                    *ptr2++ = '\b';
                    break;
                case '\f':
                    *ptr2++ = '\f';
                    break;
                case '\n':
                    *ptr2++ = '\n';
                    break;
                case '\r':
                    *ptr2++ = '\r';
                    break;
                case '\t':
                    *ptr2++ = '\t';
                    break;
                default:
                    sprintf(str, "u%04x", token);
                    ptr2++;
                    break;
            }
        }
    }

    *ptr2++ = '\"';
    *ptr2++ = '\0';

    return out;

}

char * print_array(cJSON * item, int depth, int fmt)
{
    char ** entries;
    char * out = NULL, *ptr, *ret;
    int len = 5, templen = 0, i = 0, isFail = 0;

    cJSON * child = item -> child;
    int numEntries = 0;

    while (child)
    {
        numEntries++;
        child = child -> next;
    }

    if (! numEntries)
    {
        out = (char *) cJSON_malloc(3);
        if (out)
            strcpy (out, "[]");
        return out;
    }

    entries = (char *) cJSON_malloc(numEntries * sizeof(char *));
    if (! entries)
        return NULL;
    memset(entries, 0, numEntries * sizeof(char *));

    child = item -> child;
    while (child)
    {
        ret = print_value(child, depth + 1, fmt);
        entries[i++] = ret;
        if (ret)
            len += strlen(ret) + 2 + (fmt ? 1 : 0);
        else
            isFail = 1;

        child = child -> next;
    }

    if (! isFail)
        out = (char *) cJSON_malloc(len);
    if (! out)
        isFail = 1;

    if (isFail)
    {
        for (int i=0; i<numEntries; i++)
        {
            if (entries[i])
            {
                cJSON_free(entries[i]);
            }
        }
        cJSON_free(entries);

        return NULL;
    }

    *out = '[';
    ptr = out + 1;
    *ptr = '\0';
    for (i = 0; i<numEntries; i++)
    {
        templen = strlen(entries[i]);
        memcpy(ptr, entries[i], templen);
        ptr = templen;

        if (i != numEntries - 1)
        {
            *ptr++ = ',';
            if (fmt)
                *ptr++ = ' ';
            *ptr = '\0';
        }

        cJSON_free(entries[i]);
    }
    cJSON_free(entries);

    *ptr++ = ']';
    *ptr++ = '\0';

    return out;
}

char * print_value(cJSON * item, int depth, int fmt)
{
    char * out = NULL;
    if (! item)
        return NULL;

    switch ((item -> type) & 255)
    {
        case 0:
            out = cJSON_strdup("false");
            break;
        case 1:
            out = cJSON_strdup("true");
            break;
        case 2:
            out = cJSON_strdup("null");
            break;
        case 3:
            out = print_number(item);
            break;
        case 4:
            out = print_string(item);
            break;
        case 5:
            out = print_array();
            break;
        case 6:
            out = print_object();
            break;
        default:
            break;
    }
}

char * cJSON_Print(cJSON * item)
{
    return print_value(item, 0, 1);
}





































