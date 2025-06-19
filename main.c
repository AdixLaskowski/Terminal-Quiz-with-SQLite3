#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sqlite3.h>
#include <string.h>
#include <ctype.h>

#define TEXT_SIZE 256
#define QUESTION_COUNT 10

typedef struct Question{

    int id;
    char question[TEXT_SIZE];
    char option_a[TEXT_SIZE];
    char option_b[TEXT_SIZE];
    char option_c[TEXT_SIZE];
    char option_d[TEXT_SIZE];
    char correct_option;
    char user_option;
    bool got_point;

} Question;

char DrawMenu(void);
void StartNewQuiz(void);
void AddQuestion(void);
void RemoveQuestion(void);
void ExitApp(void);
void WaitForEnter(void);
char *GetUserName(void);
void GetRandomQuestion(Question *questions);
void AskQuestion(Question *questions, int count);
void ShowSummary(Question *questions, int count);
int GetQuestionsCount(void);

int GetQuestionsCount(void)
{
    sqlite3 *myDatabase;
    sqlite3_stmt *stmt;
    int count;

    int status = sqlite3_open("quiz.db", &myDatabase);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Can't open database: %s\n", sqlite3_errmsg(myDatabase));
        return -1;
    }

    const char *sql = "SELECT COUNT(*) FROM questions";

    status = sqlite3_prepare_v2(myDatabase, sql, -1, &stmt, NULL);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Problem with preparing statement: %s\n", sqlite3_errmsg(myDatabase));
        sqlite3_close(myDatabase);
        return -1;
    }

    if(sqlite3_step(stmt) == SQLITE_ROW)
    {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(myDatabase);

    return count;

}

void WaitForEnter(void)
{
    printf("Press enter to continue: \n");
    while(getchar() != '\n');
}

char DrawMenu(void)
{
    system("clear");
    char selectedOption;
    char buffer[4];

    printf("1. Start a new quiz\n");
    printf("2. Add a new question\n");
    printf("3. Remove a question\n");
    printf("4. Show all questions\n");
    printf("5. Exit\n");

    if(fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        selectedOption = buffer[0];
    }
    return selectedOption;
}

void StartNewQuiz()
{

    int questionsCount = GetQuestionsCount();

    if(questionsCount < 10)
    {
        fprintf(stderr, "Errror! You need at least 10 questions to start a quiz!\n");
        printf("For now you have %i questions in database.\n", questionsCount);
        WaitForEnter();
        return;
    }

    char * name = GetUserName();
    printf("It's nice to see you %s!\n", name);
    printf("\n");
    
    Question questions[QUESTION_COUNT];
    GetRandomQuestion(questions);
    AskQuestion(questions, QUESTION_COUNT);
    ShowSummary(questions, QUESTION_COUNT);
}

void GetRandomQuestion(Question *questions)
{
    sqlite3 *myDatabase;
    sqlite3_stmt *stmt;

    int status = sqlite3_open("quiz.db", &myDatabase);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Can't open the database: %s\n", sqlite3_errmsg(myDatabase));
    }

    const char *sql = "SELECT * FROM questions ORDER BY RANDOM() LIMIT ?";
    
    status = sqlite3_prepare_v2(myDatabase, sql, -1, &stmt, NULL);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error: Problem with preparing statement: %s\n", sqlite3_errmsg(myDatabase));
        sqlite3_close(myDatabase);
        return;
    }

    sqlite3_bind_int(stmt, 1, QUESTION_COUNT);

    int index = 0;

    while((status = sqlite3_step(stmt)) == SQLITE_ROW && index < QUESTION_COUNT)
    {
        questions[index].id = sqlite3_column_int(stmt, 0);
        strncpy(questions[index].question, (const char *)sqlite3_column_text(stmt, 1), sizeof(questions[index].question));
        strncpy(questions[index].option_a, (const char*)sqlite3_column_text(stmt, 2), sizeof(questions[index].option_a));
        strncpy(questions[index].option_b, (const char*)sqlite3_column_text(stmt, 3), sizeof(questions[index].option_b));
        strncpy(questions[index].option_c, (const char *)sqlite3_column_text(stmt, 4), sizeof(questions[index].option_c));
        strncpy(questions[index].option_d, (const char*)sqlite3_column_text(stmt, 5), sizeof(questions[index].option_d));

        const unsigned char *correct = sqlite3_column_text(stmt, 6);

        if (correct != NULL) {
            questions[index].correct_option = correct[0];
        } else {
            questions[index].correct_option = ' ';
        }

        questions[index].user_option = ' ';
        questions[index].got_point = false;

        index++;
    }


    sqlite3_finalize(stmt);
    sqlite3_close(myDatabase);

}

char * GetUserName(void)
{
    static char name[100];
    printf("Enter your name: \n");

    if (fgets(name, sizeof(name), stdin) != NULL) {
        size_t len = strlen(name);
        if (len > 0 && name[len - 1] == '\n') {
            name[len - 1] = '\0';
        }
    }

    system("clear");
    return name;
}

void AskQuestion(Question *questions, int count)
{
    char buffer[8];

    for(int i=0; i<count; i++)
    {
        system("clear");

        printf("Question %i/%i:\n", i+1, QUESTION_COUNT);
        printf("%s\n", questions[i].question);
        printf("A) %s\n", questions[i].option_a);
        printf("B) %s\n", questions[i].option_b);
        printf("C) %s\n", questions[i].option_c);
        printf("D) %s\n", questions[i].option_d);
        printf("\n");

        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        questions[i].user_option = toupper(buffer[0]);
        questions[i].got_point = (questions[i].user_option == questions[i].correct_option);
    }

}

void ShowSummary(Question *questions, int count)
{
    int points = 0;

    system("clear");

    printf("__RESULTS__\n");
    printf("\n");

    for(int i = 0; i < count; i++)
    {
        printf("Question %i/%i\n", i+1, QUESTION_COUNT);
        printf("Question %d: %s\n", i + 1, questions[i].question);
        printf("A) %s\n", questions[i].option_a);
        printf("B) %s\n", questions[i].option_b);
        printf("C) %s\n", questions[i].option_c);
        printf("D) %s\n", questions[i].option_d);
        printf("Your answer: %c\n", questions[i].user_option);
        printf("Correct answer: %c\n", questions[i].correct_option);
        printf("%s\n", questions[i].got_point ? "+ 1 point" : "WRONG");
        printf("\n");

        if (questions[i].got_point)
            points++;
    }

    printf("===== Total Score: %d / %d =====\n", points, count);
    WaitForEnter();

}

void AddQuestion(void)
{
    sqlite3 *myDatabase;
    sqlite3_stmt *stmt;

    int status = sqlite3_open("quiz.db", &myDatabase);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Can't open tha database: %s\n", sqlite3_errmsg(myDatabase));
        return;
    }

    const char * sql = "INSERT INTO questions (question, option_a, option_b, option_c, option_d, correct_option)"
    " VALUES (?, ?, ?, ?, ?, ?)";

    status = sqlite3_prepare_v2(myDatabase, sql, -1, &stmt, NULL);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Problem with preparing statement: %s\n", sqlite3_errmsg(myDatabase));
        sqlite3_close(myDatabase);
        return;
    }

    // buffors
    char question[256];
    char option_a[128];
    char option_b[128];
    char option_c[128];
    char option_d[128];
    char correct_option[8];

    printf("Enter the question:\n");
    fgets(question, sizeof(question), stdin);

    printf("Enter option A:\n");
    fgets(option_a, sizeof(option_a), stdin);

    printf("Enter option B:\n");
    fgets(option_b, sizeof(option_b), stdin);

    printf("Enter option C:\n");
    fgets(option_c, sizeof(option_c), stdin);

    printf("Enter option D:\n");
    fgets(option_d, sizeof(option_d), stdin);

    printf("Enter correct option (A/B/C/D):\n");
    fgets(correct_option, sizeof(correct_option), stdin);

    question[strcspn(question, "\n")] = 0;
    option_a[strcspn(option_a, "\n")] = 0;
    option_b[strcspn(option_b, "\n")] = 0;
    option_c[strcspn(option_c, "\n")] = 0;
    option_d[strcspn(option_d, "\n")] = 0;
    correct_option[strcspn(correct_option, "\n")] = 0;

    sqlite3_bind_text(stmt, 1, question, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, option_a, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, option_b, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, option_c, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, option_d, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, correct_option, -1, SQLITE_TRANSIENT);


    status = sqlite3_step(stmt);

    if(status == SQLITE_DONE)
    {
        printf("Question added successfully.\n");
    }else{

        fprintf(stderr, "Error! Problem with statement's execution: %s\n", sqlite3_errmsg(myDatabase));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(myDatabase);

    WaitForEnter();
}

void DeleteQuestionFromDB(int idToDelete)
{
    // Remove a question from the database

    sqlite3 *myDatabase;
    sqlite3_stmt *stmt;

    int status = sqlite3_open("quiz.db", &myDatabase);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Problem with connecting to databse! %s", sqlite3_errmsg(myDatabase));
        return;
    }

    const char *sql = "DELETE FROM questions WHERE ID == ?";

    status = sqlite3_prepare_v2(myDatabase, sql, -1, &stmt, NULL);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Problem with statement: %s\n", sqlite3_errmsg(myDatabase));
        sqlite3_close(myDatabase);
        return;
    }

    status = sqlite3_bind_int(stmt, 1, idToDelete);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Problem with binding to ID: %s\n", sqlite3_errmsg(myDatabase));
        sqlite3_finalize(stmt);
        sqlite3_close(myDatabase);
        return;
    }

    status = sqlite3_step(stmt);
    if(status == SQLITE_DONE)
    {
        printf("Question with ID %i deleted successfully.\n", idToDelete);

    }else {

        fprintf(stderr, "Error! Can't delete question with ID %i", idToDelete);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(myDatabase);
}

void RemoveQuestion(void)
{
    int questionId;
    char buffer[16];
    char *endptr;

    printf("Enter ID of the question, you want to delete: \n");

    fgets(buffer, sizeof(buffer), stdin);

    questionId = atoi(buffer);
    if(questionId >= 0)
    {
        DeleteQuestionFromDB(questionId);
    }else{

        fprintf(stderr, "Error! Given ID is out of range!");
        return;
    }

    WaitForEnter();
}

void ShowAllQuestions(void)
{
    sqlite3 * myDatabase;
    sqlite3_stmt *stmt;

    int status = sqlite3_open("quiz.db", &myDatabase);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Can't connect to database %s\n", sqlite3_errmsg(myDatabase));
    }

    const char *sql = "SELECT * FROM questions";

    status = sqlite3_prepare_v2(myDatabase, sql, -1, &stmt, NULL);

    if(status != SQLITE_OK)
    {
        fprintf(stderr, "Error! Problem with statement: %s\n", sqlite3_errmsg(myDatabase));
        sqlite3_close(myDatabase);
        return;
    }

    while((status = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *question = sqlite3_column_text(stmt, 1);
        const unsigned char *a = sqlite3_column_text(stmt, 2);
        const unsigned char *b = sqlite3_column_text(stmt, 3);
        const unsigned char *c = sqlite3_column_text(stmt, 4);
        const unsigned char *d = sqlite3_column_text(stmt, 5);
        const unsigned char *correct = sqlite3_column_text(stmt, 6);

        printf("ID: %d\n", id);
        printf("QUESTION: %s\n", question);
        printf("A) %s\n", a);
        printf("B) %s\n", b);
        printf("C) %s\n", c);
        printf("D) %s\n", d);
        printf("Correct: %s\n", correct);
        printf("\n");
    }

    if(status != SQLITE_DONE)
    {
        fprintf(stderr, "Error! Problem with reading rows: %s\n", sqlite3_errmsg(myDatabase));
    }
    

    sqlite3_finalize(stmt);
    sqlite3_close(myDatabase);

    WaitForEnter();
}

void ExitApp(void)
{
    exit(0);
}

int main(void)
{

    while(true)
    {
        system("clear");
        char selectedOption = DrawMenu();

        switch(selectedOption)
        {
            case '1':
                system("clear");
                StartNewQuiz();
            break;

            case '2':
                system("clear");
                AddQuestion();
            break;

            case '3':
                system("clear");
                RemoveQuestion();
            break;

            case '4':
                system("clear");
                ShowAllQuestions();
            break;

            case '5':
                system("clear");
                ExitApp();
            break;

            default:
                printf("Error! No such option in the menu.");
            break;    
        }
    }

    return 0;
}