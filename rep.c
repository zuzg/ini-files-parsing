#include <stdio.h> //
#include <string.h> //eg. strtok
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h> //isalnum
#include <stdbool.h>
#define STRSTEP 10


/// sa 2 funkcje do usuwania list i tu dzialaja, ale jednak valgrindowi sie nie podoba



/*przykladowe inputy:

NORMAL
example-4.5.ini fumbling-lavish-upset-gummy-safe-straight-factor.peppery-personal-light-dead-haunting-town
example-4.5.ini fatherly-skeletal-exhausted-brief-unit.dizzy-illiterate-itchy-sweaty-fitting-floor

EXPRESSIONS
example-4.5.ini expression "distance.velocity * travel.time"
PATH-TO-INI-FILE.ini expression "distance.velocity * travel.time"
example-4.5.ini expression "fumbling-lavish-upset-gummy-safe-straight-factor.peppery-personal-light-dead-haunting-town - fatherly-skeletal-exhausted-brief-unit.dizzy-illiterate-itchy-sweaty-fitting-floor"
example-4.5.ini expression "luxurious-cylindrical-unlucky-mortified-man.overdue-repentant-minty-turbulent-handle + arid-bubbly-complicated-anguished-member.aromatic-worthwhile-luminous-silly-junior"
*/

struct string_arr{
    char * Name;
    struct string_arr * next;
};

struct Section{
    char * Sname;
    struct string_arr * Skey;
    struct string_arr * Svalue;
    struct Section * next;
};

char * getlongline(FILE * f)
{
    size_t size = STRSTEP;
    size_t last = 1; //at the beginning there is only 1 char stored in long_line

    char * long_line = malloc(sizeof(char));
    *long_line = getc(f);

    if (*long_line == '\n' || *long_line == EOF || *long_line == ';') return "empty";

    do {
        long_line = realloc(long_line,size);
        fgets(long_line+last, STRSTEP, f);
        last = strlen(long_line);
        size += STRSTEP;
    } while (!feof(f) && long_line[last-1]!='\n');

    long_line[last-1]='\0';
    return long_line;
}

char * getSectionName (char * line)
{
    char * name = strtok(line, "[");
    name[strlen(name)-1] = '\0';
    return name;
}

void Insert_Section (struct Section ** head_ref, struct Section * new_node) {
    struct Section * current = *head_ref;
    /* Special case for the head end */
    if (*head_ref == NULL ) {
        *head_ref = new_node;
    }

    else {
        /* Locate the node before the point of insertion */
        while (current->next != NULL) {current = current->next;}
        current->next = new_node;
    }
}

void Insert_Key_or_Value (struct string_arr ** head_ref, struct string_arr * new_node)
{
    struct string_arr * current = *head_ref;
    if (*head_ref == NULL ) {
        *head_ref = new_node;
    }

    else {
        while (current->next != NULL) {current = current->next;}
        current->next = new_node;
    }
}

void printList(struct Section * head) { //GOOD :D
    struct Section * n = head;
    struct string_arr * temp_key;
    struct string_arr * temp_val;

    while (n != NULL) {
        printf("\n*%s*\n", n->Sname);
        temp_key = n->Skey;
        temp_val = n->Svalue;

        while (temp_key!= NULL && temp_val!= NULL)
        {
            printf("k: (%s) v: (%s)\n", temp_key->Name, temp_val->Name);
            temp_key = temp_key->next;
            temp_val = temp_val->next;
        }
        n = n->next;
    }
}

void deleteArr(struct string_arr** head_ref) {
   /* deref head_ref to get the real head */
   struct string_arr* current = *head_ref;
   struct string_arr* next;

   while (current != NULL){
       next = current->next;
       free(current->Name);
       free(current);
       current = next;
   }

   /* deref head_ref to affect the real head back
      in the caller. */
   *head_ref = NULL;
}

void deleteList(struct Section** head_ref) {
   /* deref head_ref to get the real head */
   struct Section* current = *head_ref;
   struct Section* next;

   while (current != NULL){
       next = current->next;
       deleteArr(&current->Skey);
       free(current->Sname);
       deleteArr(&current->Svalue);
       free(current);
       current = next;
   }

   /* deref head_ref to affect the real head back
      in the caller. */
   *head_ref = NULL;
}


struct Section * DATA_READER(char * fname)
{
    FILE *fp;
    if(NULL == (fp = fopen(fname, "r"))){
        perror("File does not open");
        return NULL;
    }

    struct Section * head = NULL;
    struct Section * new_sect = NULL;

    struct string_arr * root_key = NULL;
    struct string_arr * root_value = NULL;

    struct string_arr * temp_key_node;
    struct string_arr * temp_val_node;

    char * line;
    char * tempSect;
    char * temp_key;
    char * temp_value;

    bool po_sekcji = false; //sekcja dopiero sie zaczyna

    while (!feof(fp))
    {
        line = getlongline(fp);
        if (strcmp(line, "empty")){
            if (line[0]=='['){
                if (po_sekcji){
                    new_sect = calloc(sizeof(*new_sect), 1);
                    new_sect->Sname = tempSect;
                    new_sect->Skey = root_key;
                    new_sect->Svalue = root_value;
                    Insert_Section(&head, new_sect);

                    root_key = NULL;
                    root_value = NULL;

                }
                tempSect  = getSectionName(line);
                //printf("SEKCJA *%s*\n", tempSect);

            }

            else {
                temp_key = strtok(line, " ");
                temp_value = strtok(NULL, " "); //nie wiem jak to zrobic ladniej xd ale dziala tak
                temp_value = strtok(NULL, "\0");

                temp_key_node = calloc(sizeof(*temp_key_node), 1);
                temp_key_node->Name = temp_key;
                Insert_Key_or_Value(&root_key, temp_key_node);

                temp_val_node = calloc(sizeof(*temp_val_node), 1);
                temp_val_node->Name = temp_value;
                Insert_Key_or_Value(&root_value, temp_val_node);

                po_sekcji = true;
                //printf ("%s (%s) (%s)\n", tempSect, temp_key, temp_value);
            }
        }
    }

    //ostatnia sekcja
    new_sect = calloc(sizeof(*new_sect), 1);
    new_sect->Sname = tempSect;
    new_sect->Skey = root_key;
    new_sect->Svalue = root_value;
    Insert_Section(&head, new_sect);

    fclose(fp);
    return head;
}

struct Section * Find_Section (struct Section * head, char * name){
    struct Section* current = head;
    while (current != NULL){
        if (!strcmp(current->Sname, name)){
            return current;
        }
        current = current->next;
    }
    return NULL;
}

char * Find_Value (struct Section * head, char * key){
    struct string_arr * temp_key = head->Skey;
    struct string_arr * temp_val = head->Svalue;

    while (temp_key != NULL & temp_val != NULL){
        if (!strcmp(temp_key->Name, key)){
            return temp_val->Name;
        }
        temp_key = temp_key->next;
        temp_val = temp_val->next;
    }
    return "failed";
}

bool Chars_Digits (char * word) {
    for (int i=0; i<strlen(word); i++)
            if (!isalnum(word[i]) && word[i] != '-')
                return false;
    return true;
}

int Identifier_checker(struct Section * head){
    struct Section * n = head;
    struct string_arr * temp_key;
    struct string_arr * temp_val;
    int invalid = 0;
    while (n != NULL) {
        temp_key = n->Skey;
        temp_val = n->Svalue;
        if (!Chars_Digits(n->Sname)) {printf("section name: %s\n", n->Sname); invalid++;}
        while (temp_key!= NULL && temp_val!= NULL)
        {
            if (!Chars_Digits(temp_key->Name)) {printf("key: %s\n", temp_key->Name); invalid++;}
            if (!Chars_Digits(temp_val->Name)) {printf("value: %s\n", temp_val->Name); invalid++;}
            temp_key = temp_key->next;
            temp_val = temp_val->next;
        }
        n = n->next;
    }
    return invalid;
}

bool isnumber(char *val){
    for(int i = 0; i < strlen(val); i++)
        if(!isdigit(val[i]))
            return false; //value is a string
    return true; //value is a number
}

void calc(char *val1, char *val2, char op) {
    int n1 = atoi(val1);
    int n2 = atoi(val2);
    printf("RESULT: ");
    switch(op){
        case '+':
            printf("%d + %d = %d\n", n1, n2, n1+n2);
            break;

        case '-':
            printf("%d - %d = %d\n", n1, n2, n1-n2);
            break;

        case '*':
            printf("%d * %d = %d\n", n1, n2, n1*n2);
            break;

        case '/':
            printf("%d / %d = %d\n", n1, n2, n1/n2);
            break;

        default:
            printf("Error! operator is not correct\n");
    }
}

void expression(char *val1, char *val2, char op) {
    if(isnumber(val1) != isnumber(val2))
        printf("Invalid expression - different types of data.\n");

    else if (isnumber(val1) && isnumber(val2))
        calc(val1, val2, op);

    else {
        if(op == '+'){
            char *result = malloc(strlen(val1) + strlen(val2) + 1); // +1 for the null-terminator
            strcpy(result, val1);
            strcat(result, val2);
            printf("Concatened string: %s\n", result);
            free(result);
        }
        else
            printf("Unsupported expression for string type.\n");
    }
}

int main ()
{
    char * input = getlongline(stdin);
    printf("\n");

    /// jesli mamy "expression" w inpucie to robimy wersje na 5
    if(strstr(input, "expression") && strstr(input, "\"")) {
        char * filename = strtok(input, " ");
        char * skip = strtok(NULL, "\"");
        char * section1 = strtok(NULL, ".");
        char * key1 = strtok(NULL, " ");
        char * op = strtok(NULL, " ");
        char * section2 = strtok(NULL, ".");
        char * key2 = strtok(NULL, "\"");
        //printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n", filename, skip, section1, key1, op, section2, key2);

        //char op = oper[0];
        //printf("%c", op[0]);
        struct Section * DATA = DATA_READER(filename);
        if (DATA == NULL) return -1;

        struct Section * searched_section1 = Find_Section(DATA, section1);
        if (searched_section1 == NULL){
            printf("Failed to find section [%s]\n", section1);
        }
        struct Section * searched_section2 = Find_Section(DATA, section2);
        if (searched_section2 == NULL){
            printf("Failed to find section [%s]\n", section2);
        }
        else{
            char * value1 = Find_Value (searched_section1, key1);
            if (strcmp(value1, "failed"))
                printf("FOUND! value1: %s\n", value1);
            else
                printf("Failed to find key \"%s\" in section [%s]\n", key1, section1);

            char * value2 = Find_Value (searched_section2, key2);
            if (strcmp(value2, "failed"))
                printf("FOUND! value2: %s\n", value2);
            else
                printf("Failed to find key \"%s\" in section [%s]\n", key2, section2);

            if (strcmp(value1, "failed") && strcmp(value2, "failed"))
                //op[0] zeby byl char
                expression(value1, value2, op[0]);
        }
        printf("Detecting invalid identifiers...\n");
        int invalid = Identifier_checker(DATA);
        (invalid==0) ? printf("\n\nAll identifiers are correct!\n") : printf("SUMMARY: Found %d invalid identifier(s)\n", invalid);

        deleteList(&DATA);
        deleteList(&searched_section1);
        deleteList(&searched_section2);
        //test czy cos zostalo w liscie
        printf("[");
        printList(searched_section1);
        printf("]");
    }
    /// to pozostaje bez zmian - wersja dla 1 szukanej sekcji
    else {
        char * filename = strtok(input, " ");
        char * section = strtok(NULL, ".");
        char * key = strtok(NULL, "\0");

        struct Section * DATA = DATA_READER(filename);
        if (DATA == NULL) return -1;

        struct Section * searched_section = Find_Section(DATA, section);
        if (searched_section == NULL){
            printf("Failed to find section [%s]\n", section);
        }
        else{
            char * value = Find_Value (searched_section, key);
            if (strcmp(value, "failed"))
                printf("FOUND! value: %s\n", value);
            else
                printf("Failed to find key \"%s\" in section [%s]\n", key, section);
        }
        printf("Detecting invalid identifiers...\n");
        int invalid = Identifier_checker(DATA);
        (invalid==0) ? printf("\n\nAll identifiers are correct!\n") : printf("SUMMARY: Found %d invalid identifier(s)\n", invalid);

        deleteList(&DATA);
        deleteList(&searched_section);
        printf("[");
        printList(DATA);
        printf("]");
    }

    //printList(DATA);
    printf("\nWIELKANOCNY B4 przed party :D :-P ;))");



    return 0;
}