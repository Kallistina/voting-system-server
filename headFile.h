#include <iostream> //all the needed libraries
#include <vector>
#include <string>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <pthread.h>    /* For threads */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h>
#include <unistd.h> /* fork */
#include <ctype.h>  /* toupper */
#include <signal.h> /* signal */
#include <algorithm>
void child_server(int newsock);
void perror_exit(char *message);
void sigchld_handler(int sig);

using namespace std;

typedef struct voter_ voter; // struct to keep seperately name, last name and voted party of a voter
struct voter_
{
    string name;
    string last_name;
    string political_party;
};

typedef struct political_party_ pparty; // struct to keep the political party's name and the total votes it gathered in the elections
struct political_party_
{
    string name;
    int votes;
};