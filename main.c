#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wait.h>

#define BUFF_SIZE 256

const char *myfifo = "/tmp/myfifo";
const char *log_file_name = "log";
char *name_prog;

void writeToLog(char *str, int log_file) {
    char s[BUFF_SIZE];
    sprintf(s, "%s:  %s\n", name_prog, str);
    write(log_file, s, strlen(s));
}

void writeError(char *err, int log_file) {
    perror(err);
    writeToLog(err, log_file);
}

void killByPid(int pid, int log_file) {
    char command[BUFF_SIZE];
    sprintf(command, "kill %d", pid);
    system(command);
    writeToLog(command, log_file);
}

int checkActive(char *name_of_file) {
    //checking existing of pipe
    if (access(myfifo, F_OK) == -1) {
        mkfifo(myfifo, 0666);
        return 0;
    }
    return 1;
}

void writeToPipe(int fd, int log_file){
    //writing to pipe
    char str_pid[BUFF_SIZE];
    writeToLog("Start writing", log_file);
    sprintf(str_pid, "%d", getpid());
    ssize_t byte_write = write(fd, str_pid, strlen(str_pid));
    char tempstr[BUFF_SIZE];
    sprintf(tempstr, "Write pid = %d", getpid());
    writeToLog(tempstr, log_file);
}

void readFromPipe(int fd, int log_file){
    int kill_pid;
    char buf[BUFF_SIZE];
    writeToLog("Start reading", log_file);
    ssize_t byte_read = read(fd, buf, BUFF_SIZE);
    switch (byte_read) {
        case -1:
            if (errno == EAGAIN) {
                writeError("Empty pipe! Write before reading", log_file);
                break;
            } else {
                writeError("Reading pipe error.", log_file);
                exit(-3);
            }
        default:
            kill_pid = atoi(buf);
            killByPid(kill_pid, log_file);
    }
    //after reading we must write, but after writing we couldn't read
    writeToPipe(fd, log_file);
}

int main(int argc, char *argv[]) {
    //argv[0] - name of file
    //argv[1] - your name of program
    //argv[2...] - will be ignored
    if (argc == 1) {
        printf("Set the name of the program");
        return 1;
    }
    if (strcmp(argv[1], "clear") == 0){
        system("rm -rf /tmp/myfifo log*");
        return 0;
    }
    name_prog = argv[1];
    //some kind of switcher
    //0 - first write
    //1 - first read
    int flag_write = checkActive(argv[0]);

    //open log file
    char tempstr[BUFF_SIZE];
    sprintf(tempstr, "%s%s", log_file_name, name_prog);

    int log_file = open(tempstr, O_WRONLY | O_CREAT, 0666);
    if (log_file < 0) {
        perror("Can't open log file!\n");
        exit(1);
    }

    writeToLog("Started", log_file);

    //open pipe
    int fd;
    fd = open(myfifo, O_RDWR);
    if (fd < 0) {
        writeError("Can't open pipe.", log_file);
        exit(-1);
    }

    int ppid;
    while (1) {

        ppid = getpid();
        switch (fork()) {
            case -1:
                exit(-4);
            case 0:
                //Child will wait for ending of parent
                memset(tempstr, 0, strlen(tempstr));
                sprintf(tempstr, "Child pid = %d", getpid());
                writeToLog(tempstr, log_file);
                while (kill(ppid, 0) == 0);
                writeToLog("Start new circle!", log_file);
                break;
            default:
                if(flag_write == 1)
                    readFromPipe(fd, log_file);
                else
                    //for the first process only!
                    writeToPipe(fd, log_file);
                while (1);
        }
        flag_write = 1;
    }
    return 0;
}