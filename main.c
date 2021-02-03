//
// Created by 32119 on 2021/2/3.
//

#include <stdlib.h>
#include <stdio.h>

#include "c_json.h"

int main()
{
    cJSON * root = cJSON_CreateObject();
    cJSON * node1 = cJSON_CreateObject();
    cJSON * node2 = cJSON_CreateObject();
    cJSON * node3 = cJSON_CreateObject();

    cJSON_AddFalseToObject(node1, "node1_key1");
    cJSON_AddTrueToObject(node1, "node1_key2");

    cJSON_AddStringToObject(node2, "node2_key1", "node2_value1");
    cJSON_AddStringToObject(node2, "node2_key2", "node2_value2");

    cJSON_AddNumberToObject(node3, "node3_key1", 1000);
    cJSON_AddNumberToObject(node3, "node3_key2", 2000);

    cJSON_AddItemToObject(root, "root_node1", node1);
    cJSON_AddItemToObject(root, "root_node2", node2);

    cJSON_AddItemToObject(node1, "node1_node3", node3);

    // �����root���Ǵ�������json����
    char * buf = cJSON_Print(root);
    printf("json:\n%s\n", buf);
    cJSON * parse_node = cJSON_Parse(buf);

    cJSON_Delete(root);
    cJSON_Delete(parse_node);
    free(buf);

    system("pause");
    return 0;
}













