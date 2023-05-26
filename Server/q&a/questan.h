#define MAX_ANSWER 100
#define MAX_LEGTH 1024

typedef struct {
    char pergunta[MAX_LEGTH];
    char user[12];
} Answer;

typedef struct{
    Answer answers[MAX_ANSWER];
    char question[MAX_LEGTH];
}Question;