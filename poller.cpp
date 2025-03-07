#include "headFile.h"

ofstream pfile; // poll-log file
ofstream sfile; // poll-stats file

int *buff;
int b_start = 0; // buff indexes
int b_end = 0;
int bufferSize;

vector<string> voters;
vector<pparty> political_parties;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t common_mutex = PTHREAD_MUTEX_INITIALIZER;
vector<string> fileLines;

pthread_cond_t cvar_full, cvar_empty; /* Condition variable */

void *thread_f(void *argp);
void terminate(int signum);

int main(int argc, char *argv[])
{
    signal(SIGINT, terminate);
    // save program's args
    int portNum = atoi(argv[1]);
    int numWorkerthreads = atoi(argv[2]);
    bufferSize = atoi(argv[3]);
    if (bufferSize <= 0)
    {
        cout << "Error: Wrong bufferSize" << endl;
        exit(1);
    }

    buff = (int *)malloc(sizeof(int) * bufferSize);

    string poll_log = argv[4];
    string poll_stats = argv[5];

    pfile.open(poll_log);
    sfile.open(poll_stats);

    pthread_cond_init(&cvar_full, NULL);  /* Initialize condition variable */
    pthread_cond_init(&cvar_empty, NULL); /* Initialize condition variable */

    for (int i = 0; i < numWorkerthreads; i++)
    {
        pthread_t thr;
        int err;
        if (err = pthread_create(&thr, NULL, thread_f, NULL))
        {
            perror("pthread_create");
            exit(2);
        }
    }

    int sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;

    struct sockaddr *clientptr = (struct sockaddr *)&client;
    struct hostent *rem;

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) /* Create socket */
    {
        perror("socket");
        exit(1);
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portNum); /* The given port */

    if (bind(sock, serverptr, sizeof(server)) < 0) /* Bind socket to address */
    {
        perror("bind");
        exit(1);
    }

    if (listen(sock, 100) < 0) /* Listen for connections */
    {
        perror("listen");
        exit(1);
    }

    printf("Listening for connections to port % d\n", portNum);

    while (1)
    {
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0)
        {
            perror("accept"); /* accept connection */
            exit(1);
        }

        pthread_mutex_lock(&mutex);
        while ((b_end == b_start - 1) || (b_start == 0 && b_end == bufferSize - 1))
        {
            pthread_cond_wait(&cvar_full, &mutex);
        }
        buff[b_end] = newsock;
        b_end++;
        if (b_end == bufferSize)
            b_end = 0;

        pthread_cond_signal(&cvar_empty);
        pthread_mutex_unlock(&mutex);
    }
}

void terminate(int signum)
{
    int total_votes = 0;
    pthread_mutex_lock(&common_mutex);
    while (political_parties.size() != 0)
    {
        int max_votes = 0;
        int index = 0;
        for (int i = 0; i < political_parties.size(); i++)
        {
            if (political_parties[i].votes > max_votes)
            {
                max_votes = political_parties[i].votes;
                index = i;
            }
        }
        total_votes += political_parties[index].votes;
        sfile << political_parties[index].name << " " << political_parties[index].votes << "\n";
        political_parties.erase(political_parties.begin() + index);
    }
    sfile << "TOTAL " << total_votes << "\n";
    pthread_mutex_unlock(&common_mutex);
    exit(0);
}

void *thread_f(void *argp) /* Thread function */
{
    while (1)
    {
        int sock;
        pthread_mutex_lock(&mutex);
        while (b_start == b_end) // empty buffer
        {
            pthread_cond_wait(&cvar_empty, &mutex);
        }
        sock = buff[b_start];
        b_start++;
        if (b_start == bufferSize) // if buffer is full
            b_start = 0;

        pthread_cond_signal(&cvar_full);

        pthread_mutex_unlock(&mutex);
        // WRITE SEND NAME PLEASE
        string message1 = "SEND NAME PLEASE";
        write(sock, message1.c_str(), message1.length());
        // READ VOTER'S NAME
        char message2[2];
        string message_s = "";

        read(sock, message2, 1);

        while (message2[0] != '$')
        {
            message_s += message2[0];
            read(sock, message2, 1);
        }
        string name = message_s; // voter's name
                                 // CHECK IF THIS NAME-LAST NAME HAS ALREADY VOTED
        pthread_mutex_lock(&common_mutex);
        bool already_voted = 0;

        if (find(voters.begin(), voters.end(), message_s) != voters.end()) // check if the voter has already voted
            already_voted = 1;
        else
            voters.push_back(name); // save the new voter
        // WRITE ALREADY VOTED
        pthread_mutex_unlock(&common_mutex);
        if (already_voted)
        {
            string message3 = "ALREADY VOTED";
            write(sock, message3.c_str(), message3.length());
            close(sock);
            continue; // continue to the next voter
        }
        // WRITE SEND VOTE PLEASE
        string message4 = "SEND VOTE PLEASE";
        write(sock, message4.c_str(), message4.length());

        message_s = "";
        // READ THE VOTER'S VOTE
        read(sock, message2, 1);

        while (message2[0] != '$')
        {
            message_s += message2[0];
            read(sock, message2, 1);
        }

        string political_party = message_s; // voter voted this political party

        string message5 = "VOTE for Party " + political_party + " RECORDED";
        write(sock, message5.c_str(), message5.length());

        close(sock);

        pthread_mutex_lock(&common_mutex);
        pfile << name << " " << political_party << "\n"; // save data to poll-log file

        bool found = 0; // check if this party is already saved in the vector
        for (int i = 0; i < political_parties.size(); i++)
        {
            if (political_parties[i].name == political_party)
            {
                political_parties[i].votes++; // if found, inform its votes
                found = 1;
                break;
            }
        }
        if (found == 0) // if it isnt, save it and informed its votes
        {
            pparty new_party;
            new_party.name = political_party;
            new_party.votes = 1;
            political_parties.push_back(new_party);
        }

        pthread_mutex_unlock(&common_mutex);
    }
}