
/* a struct with one int */
$remote typedef struct { 
    int style;
} intStruct;		

$remote typedef struct {
    int computer;
    int science;
} mStruct;

/*
 *
 */
$remote test {
    $base 2000;
    $version 1;
    void nullNull();				// a comment
    int intNull();
    void nullStruct(intStruct);
    int intString(char *);
    int add(int, int);
    int sumVector($array int);
    $array int retVector(int len, int start);
    char *stringString(char *);
    void triggerAsyncUpcall(int val);
    $upcall $async void asyncUpcall(int x);
};
