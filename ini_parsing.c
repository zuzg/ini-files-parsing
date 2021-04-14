#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>
#define STRSTEP 10
#pragma warning(disable : 4996) // needed in VS

/// Agata Zywot 148258
/// Zuzanna Gawrysiak 148255

/*sample inputs:

NORMAL
example-4.5.ini fumbling-lavish-upset-gummy-safe-straight-factor.peppery-personal-light-dead-haunting-town
example-4.5.ini fatherly-skeletal-exhausted-brief-unit.dizzy-illiterate-itchy-sweaty-fitting-floor

EXPRESSIONS
example-4.5.ini expression "distance.velocity * travel.time"
PATH-TO-INI-FILE.ini expression "distance.velocity * travel.time"
example-4.5.ini expression "fumbling-lavish-upset-gummy-safe-straight-factor.peppery-personal-light-dead-haunting-town - fatherly-skeletal-exhausted-brief-unit.dizzy-illiterate-itchy-sweaty-fitting-floor"
example-4.5.ini expression "luxurious-cylindrical-unlucky-mortified-man.overdue-repentant-minty-turbulent-handle + arid-bubbly-complicated-anguished-member.aromatic-worthwhile-luminous-silly-junior"
*/

struct string_arr {
    char* Name;
    struct string_arr* next;
};

struct Section {
    char* Sname;
    struct string_arr* Skey;
    struct string_arr* Svalue;
    struct Section* next;
};

char* getlongline(FILE* f)
{
    size_t size = STRSTEP;
    size_t last = 1; //at the beginning there is only 1 char stored in long_line

    char* long_line, * tmp;
    long_line = (char*)malloc(sizeof(char)); //1
    *long_line = getc(f);

    if (*long_line == '\n' || *long_line == EOF || *long_line == ';') {
        free(long_line);
        return "empty";
    }

    do {
        if (long_line != NULL){
            tmp = (char*)realloc(long_line, last + size);
            if (tmp != NULL)
                long_line = tmp;
        }
        fgets(long_line + last, STRSTEP, f);
        last = strlen(long_line);
        size += STRSTEP;
    } while (!feof(f) && long_line[last - 1] != '\n' && long_line);

    long_line[last - 1] = '\0';
    return long_line;
}

char* getSectionName(char* line)
{
    char* name = (char*)calloc(1, strlen(line));
    for (size_t i = 0; i < strlen(line) - 2; i++)
        name[i] = line[i + 1];

    name[strlen(line) - 2] = '\0';
    return name;
}

void Insert_Section(struct Section** head_ref, char* sect_name, struct string_arr* keys, struct string_arr* values) {

    struct Section* new_node = calloc(1, sizeof(struct Section));

    if (new_node)
    {
        new_node->Sname = malloc(strlen(sect_name) + 1);
        strcpy(new_node->Sname, sect_name);
        new_node->Skey = keys;
        new_node->Svalue = values;

        struct Section* current = *head_ref;
        //special case for the head end
        if (*head_ref == NULL) {
            *head_ref = new_node;
        }

        else {
            //locate the node before the point of insertion
            while (current->next != NULL) { current = current->next; }
            current->next = new_node;
        }
    }
}

void Insert_Key_or_Value(struct string_arr** head_ref, char* x)
{
    struct string_arr* new_node = calloc(1, sizeof(struct string_arr));
    if (new_node)
    {
        new_node->Name = malloc(strlen(x) + 1);
        strcpy(new_node->Name, x);

        struct string_arr* current = *head_ref;
        if (*head_ref == NULL) {
            *head_ref = new_node;
        }

        else {
            while (current->next != NULL) { current = current->next; }
            current->next = new_node;
        }
    }
}

void printList(struct Section* head) {
    struct Section* n = head;
    struct string_arr* temp_key;
    struct string_arr* temp_val;

    while (n != NULL) {
        printf("\n*%s*\n", n->Sname);
        temp_key = n->Skey;
        temp_val = n->Svalue;

        while (temp_key != NULL && temp_val != NULL)
        {
            printf("k: (%s) v: (%s)\n", temp_key->Name, temp_val->Name);
            temp_key = temp_key->next;
            temp_val = temp_val->next;
        }
        n = n->next;
    }
}

void deleteArr(struct string_arr** head_ref) {
    struct string_arr* current = *head_ref;
    struct string_arr* next;

    while (current != NULL) {
        next = current->next;
        free(current->Name);
        free(current);
        current = next;
    }

    *head_ref = NULL;
}

void deleteList(struct Section** head_ref) {
    struct Section* current = *head_ref;
    struct Section* next;

    while (current != NULL) {
        next = current->next;
        deleteArr(&current->Svalue);
        deleteArr(&current->Skey);
        free(current->Sname);
        free(current);
        current = next;
    }

    *head_ref = NULL;
}

struct Section* DATA_READER(char* fname)
{
    FILE* fp;
    if (NULL == (fp = fopen(fname, "r"))) {
        perror("File does not open");
        return NULL;
    }

    struct Section* head = NULL;
    struct string_arr* root_key = NULL;
    struct string_arr* root_value = NULL;

    char* line;
    char* tempSect = NULL;
    char* temp_key;
    char* temp_value;

    bool after_section = false; //beginning of section

    while (!feof(fp)){
        line = getlongline(fp);
        //the line is not empty
        if (strcmp(line, "empty")){
            if (line[0] == '['){
                if (after_section){
                    Insert_Section(&head, tempSect, root_key, root_value);
                    free(tempSect);
                    root_key = NULL;
                    root_value = NULL;
                }
                tempSect = getSectionName(line);

            }

            else{
                temp_key = strtok(line, " ");
                temp_value = strtok(NULL, " ");
                temp_value = strtok(NULL, "\0");

                Insert_Key_or_Value(&root_key, temp_key);
                Insert_Key_or_Value(&root_value, temp_value);

                after_section = true;
            }
            free(line);
        }
    }

    //last section
    Insert_Section(&head, tempSect, root_key, root_value);
    root_key = NULL;
    root_value = NULL;
    free(tempSect);

    fclose(fp);
    return head;
}

struct Section* Find_Section(struct Section* head, char* name) {
    struct Section* current = head;
    while (current != NULL) {
        if (!strcmp(current->Sname, name))
            return current;

        current = current->next;
    }
    return NULL;
}

char* Find_Value(struct Section* head, char* key){
    struct string_arr* temp_key = head->Skey;
    struct string_arr* temp_val = head->Svalue;

    while (temp_key != NULL && temp_val != NULL) {
        if (!strcmp(temp_key->Name, key))
            return temp_val->Name;

        temp_key = temp_key->next;
        temp_val = temp_val->next;
    }
    return "failed";
}

bool Chars_Digits(char* word){
    for (size_t i = 0; i < strlen(word); i++)
        if (!isalnum(word[i]) && word[i] != '-')return false;

    return true;
}

int Identifier_checker(struct Section* head){
    struct Section* n = head;
    struct string_arr* temp_key;
    struct string_arr* temp_val;
    int invalid = 0;
    while (n != NULL) {
        temp_key = n->Skey;
        temp_val = n->Svalue;
        if (!Chars_Digits(n->Sname)) {
            printf("section name: %s\n", n->Sname);
            invalid++;
        }
        while (temp_key != NULL && temp_val != NULL){
            if (!Chars_Digits(temp_key->Name)) { printf("key: %s\n", temp_key->Name); invalid++; }
            if (!Chars_Digits(temp_val->Name)) { printf("value: %s\n", temp_val->Name); invalid++; }
            temp_key = temp_key->next;
            temp_val = temp_val->next;
        }
        n = n->next;
    }
    return invalid;
}

bool isnumber(char* val) {
    for (size_t i = 0; i < strlen(val); i++)
        if (!isdigit(val[i]))
            return false; //value is a string
    return true; //value is a number
}

void calc(char* val1, char* val2, char op) {
    int n1 = atoi(val1);
    int n2 = atoi(val2);
    printf("RESULT: ");
    switch (op) {
    case '+':
        printf("%d + %d = %d\n", n1, n2, n1 + n2);
        break;

    case '-':
        printf("%d - %d = %d\n", n1, n2, n1 - n2);
        break;

    case '*':
        printf("%d * %d = %d\n", n1, n2, n1 * n2);
        break;

    case '/':
        printf("%d / %d = %d\n", n1, n2, n1 / n2);
        break;

    default:
        printf("Error! operator is not correct\n");
    }
}

void expression(char* val1, char* val2, char op) {
    if (isnumber(val1) != isnumber(val2))
        printf("Invalid expression - different types of data.\n");

    else if (isnumber(val1) && isnumber(val2))
        calc(val1, val2, op);

    else {
        if (op == '+') {
            char* result = malloc(strlen(val1) + strlen(val2) + 1); // +1 for the null-terminator
            if (result){
                strcpy(result, val1);
                strcat(result, val2);
                printf("Concatened string: %s\n", result);
                free(result);
            }
            else printf("Error!\n");
        }
        else
            printf("Unsupported expression for string type.\n");
    }
}

int main(){
    char* input = getlongline(stdin);
    printf("\n");

    /// "expression" - 2 sections version (grade 5)
    if (strstr(input, "expression") && strstr(input, "\"")) {
        char* filename = strtok(input, " ");
        char* section1 = strtok(NULL, "\"");
        section1 = strtok(NULL, ".");
        char* key1 = strtok(NULL, " ");
        char* op = strtok(NULL, " ");
        char* section2 = strtok(NULL, ".");
        char* key2 = strtok(NULL, "\"");

        struct Section* DATA = DATA_READER(filename);
        if (DATA == NULL) return -1;

        struct Section* searched_section1 = Find_Section(DATA, section1);
        if (searched_section1 == NULL) {
            printf("Failed to find section [%s]\n", section1);
        }
        struct Section* searched_section2 = Find_Section(DATA, section2);
        if (searched_section2 == NULL) {
            printf("Failed to find section [%s]\n", section2);
        }
        else {
            char* value1 = Find_Value(searched_section1, key1);
            if (strcmp(value1, "failed"))
                printf("FOUND! value1: %s\n", value1);
            else
                printf("Failed to find key \"%s\" in section [%s]\n", key1, section1);

            char* value2 = Find_Value(searched_section2, key2);
            if (strcmp(value2, "failed"))
                printf("FOUND! value2: %s\n", value2);
            else
                printf("Failed to find key \"%s\" in section [%s]\n", key2, section2);

            if (strcmp(value1, "failed") && strcmp(value2, "failed"))
            {
                if (strlen(op)==1) expression(value1, value2, op[0]);
                else printf("Error! operator '\%s\' is not correct\n", op);
            }
        }
        printf("Detecting invalid identifiers...\n");
        int invalid = Identifier_checker(DATA);
        (invalid == 0) ? printf("\n\nAll identifiers are correct!\n") : printf("SUMMARY: Found %d invalid identifier(s)\n", invalid);

        deleteList(&DATA);
    }
    /// when looking for 1 section
    else {
        char* filename = strtok(input, " ");
        char* section = strtok(NULL, ".");
        char* key = strtok(NULL, "\0");

        struct Section* DATA = DATA_READER(filename);
        if (DATA == NULL) return -1;

        struct Section* searched_section = Find_Section(DATA, section);
        if (searched_section == NULL) {
            printf("Failed to find section [%s]\n", section);
        }
        else {
            char* value = Find_Value(searched_section, key);
            if (strcmp(value, "failed"))
                printf("FOUND! value: %s\n", value);
            else
                printf("Failed to find key \"%s\" in section [%s]\n", key, section);
        }
        printf("Detecting invalid identifiers...\n");
        int invalid = Identifier_checker(DATA);
        (invalid == 0) ? printf("\n\nAll identifiers are correct!\n") : printf("SUMMARY: Found %d invalid identifier(s)\n", invalid);

        deleteList(&DATA);
    }
    free(input);
    return 0;
}
