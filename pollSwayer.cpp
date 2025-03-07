#include "headFile.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
vector<string> fileLines;

string serverName;
int portNum;

void *thread_f(void *argp); /* Thread function */

int main(int argc, char *argv[])
{
    // save program's args
    serverName = argv[1];
    portNum = atoi(argv[2]);
    string inputFile = argv[3];
    int err;

    // open inputFile & read from it
    ifstream inFile;
    inFile.open(inputFile);
    if (!inFile)
    {
        cout << "Unable to open file:" << inputFile << endl;
        exit(1); // terminate with error
    }

    vector<pthread_t> threadIds; // save each thread id
    // save every file's line in the vector and create thread for every line
    string line;
    while (getline(inFile, line))
    {
        pthread_mutex_lock(&mutex);
        fileLines.push_back(line);
        pthread_mutex_unlock(&mutex);
        pthread_t thr;

        if (err = pthread_create(&thr, NULL, thread_f, NULL)) /* New thread */
        {
            perror("pthread_create");
            exit(2);
        }
        threadIds.push_back(thr); // save thread's id
    }

    for (int i = 0; i < threadIds.size(); i++) // wait all threads to terminate
    {
        if (err = pthread_join(threadIds[i], NULL))
        {
            perror("pthread_join");
            exit(3);
        }
    }

    inFile.close();
}

void *thread_f(void *argp)
{
    pthread_mutex_lock(&mutex);
    string line = fileLines[0];
    fileLines.erase(fileLines.begin());
    pthread_mutex_unlock(&mutex);

    voter parsedLine; // parsing the line: name-lastname-politicalparty
    int flag = 0;
    for (int i = 0; i < line.length(); i++)
    {
        if (line[i] != ' ')
        {
            if (flag == 0)
                parsedLine.name.push_back(line[i]);
            else if (flag == 1)
                parsedLine.last_name.push_back(line[i]);
            else
                parsedLine.political_party.push_back(line[i]);
        }
        else
            flag++;
    }

    int sock, i;
    char buf[256];
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) /* Create socket */
    {
        perror("socket");
        exit(1);
    }

    if ((rem = gethostbyname(serverName.c_str())) == NULL) /* Find server address */
    {
        herror("gethostbyname");
        exit(1);
    }

    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(portNum); /* Server port */

    if (connect(sock, serverptr, sizeof(server)) < 0) /* Initiate connection */
    {
        perror("connect");
        exit(1);
    }

    char message[2];
    string message_s = "";
    // read the message SEND NAME PLEASE
    while (message_s != "SEND NAME PLEASE")
    {
        read(sock, message, 1); // read byte per byte
        message_s += message[0];
    }

    string voter_name = parsedLine.name + " " + parsedLine.last_name + "$"; //$ at the end means that the message stops here

    write(sock, voter_name.c_str(), voter_name.length()); // reply to poller by sending the voter's name

    message_s = "";

    while (message_s != "SEND VOTE PLEASE" && message_s != "ALREADY VOTED")
    {
        read(sock, message, 1);
        message_s += message[0];
    }

    if (message_s == "ALREADY VOTED")
    {
        close(sock);
        pthread_exit((void *)47);
    }

    string voter_political_party = parsedLine.political_party + "$";

    write(sock, voter_political_party.c_str(), voter_political_party.length()); // reply to poller by sending the voter's vote

    close(sock);
    pthread_exit((void *)47);
}