#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMD_BUF_SZ 128

char error_message[30] = "An error has occurred\n";

#define ERR_MSG \
{\
    write(STDERR_FILENO, error_message, strlen(error_message));\
}

#define CHECK_AND_EXIT(var) \
{\
    if (!(var)) {\
        ERR_MSG\
        exit(EXIT_FAILURE);\
    }\
}

void handlePath(char *pathArgs[], char ***pathvals)
{
    int i = 0, j = 0;
    int numPaths = 0;

    // count the number of paths
    while(pathArgs[i++]) numPaths++;

    i = 0;
    // free up any old paths
    if (*pathvals) {
        while((*pathvals)[i]) {
            free((*pathvals)[i]);
            i++;
        }
        free(*pathvals);
    }

    // allocate new paths
    // Here * 2 is used as each path is setup as absolute path
    // and relative path to the current directory
    // An extra one is added to ensure that the path array
    // terminates with NULL. This ensures, we don't have to
    // maintain any path count.
    *pathvals = malloc(sizeof(char *) * ((numPaths * 2) + 1));
    CHECK_AND_EXIT((*pathvals));

    i = 0;
    j = 0;

    while(i < numPaths) {
        // setup absolute path
        (*pathvals)[j] = malloc(sizeof(char) * (strlen(pathArgs[i])+4));
        CHECK_AND_EXIT((*pathvals)[j]);
        sprintf((*pathvals)[j], "%s/", pathArgs[i]);

        // setup relative to the current directory
        (*pathvals)[j+1] = malloc(sizeof(char) * (strlen(pathArgs[i])+4));
        CHECK_AND_EXIT((*pathvals)[j+1]);
        sprintf((*pathvals)[j+1], "./%s/", pathArgs[i]);

        // adding 2 for, 2 kinds of paths
        j += 2;
        i++;
    }
    // setup the tail as NULL
    (*pathvals)[j]=NULL;
}

void handleCmds(char ***pathvals, char ***args, int numCmds)
{
    pid_t *pidList = NULL;
    int i = 0;
    char **paths = *pathvals;

    pidList = malloc(numCmds * sizeof(pid_t));
    CHECK_AND_EXIT(pidList);
    for (i = 0; i < numCmds; i++) {
        pidList[i] = 0;
    }

    for(i = 0; i < numCmds; i++) {
        if (!args[i][0]) continue;
        // Deal with built-ins
        if (!strcmp(args[i][0], "exit")) {
            if (args[i][1]) {
                ERR_MSG;
            } else {
                exit(0);
            }
        } else if (!strcmp(args[i][0], "cd")) {
            if (!args[i][1] || args[i][2]) {
                ERR_MSG;
            } else {
                if (chdir(args[i][1]) != 0) {
                    ERR_MSG;
                }
            }
        } else if (!strcmp(args[i][0], "path")) {
            handlePath(&args[i][1], pathvals);
        } else {
            // Regular shell commands
            int j = 0, path = 0;
            bool invalidArgs = false;
            size_t len = 0;
            char *cmd = NULL;
            int out, err, outBkp, errBkp, ret;
            pid_t pid;
            char *fileName = NULL;

            while(args[i][j] != NULL) {
                // check if there are any redirections in the command
                // If yes, then save the filename
                if (!strcmp(args[i][j], ">")) {
                    // to deal with cases like single ">" without filename
                    // or where ">" is the first argument
                    // or multiple files after redir argument
                    if (!args[i][j+1] || j == 0 || args[i][j+2]) {
                        invalidArgs = true;
                        break;
                    }
                    fileName = malloc(sizeof(char) * (strlen(args[i][j+1])+1));
                    CHECK_AND_EXIT(fileName);

                    sprintf(fileName, "%s", args[i][j+1]);
                    args[i][j] = NULL;
                    args[i][j+1] = NULL;
                    break;
                }
                j++;
            }

            if (invalidArgs) {
                ERR_MSG;
                continue;
            }

            // check if command already contains
            // the path
            len = strlen(args[i][0]) + 1;
            cmd = malloc(sizeof(char) * len);
            CHECK_AND_EXIT(cmd);
            memset(cmd, '\0', len);
            sprintf(cmd, "%s", args[i][0]);
            if (access(cmd, X_OK)) {
                // command couldn't be executed
                // try searching for command in path
                if (cmd) free(cmd);
                cmd = NULL;
                while(paths[path]) {
                    len = strlen(&paths[path][0]) + strlen(args[i][0]) + 1;
                    cmd = malloc(sizeof(char) * len);
                    CHECK_AND_EXIT(cmd);
                    memset(cmd, '\0', len);

                    // append path to the command and search for the command
                    // and check if the command is executable or not
                    sprintf(cmd, "%s%s", &paths[path][0], args[i][0]);
                    if (!access(cmd, X_OK)) {
                        // valid command
                        break;
                    }
                    path++;
                    free(cmd);
                    cmd = NULL;
                }
            }
            // command not found in any path, continue to next one
            if (!cmd) {
                ERR_MSG;
                if (fileName)
                    free(fileName);
                continue;
            }
            if (fileName != NULL) {
                // open the redir file as out and err
                // save the file descriptors of stdout and stderr
                // copy the opened file descriptors to stdout and stderr
                // On forking the child inherits the parent file descriptors
                // hence child will inherit these open descriptors and write
                // to the file
                out = open(fileName, O_RDWR|O_CREAT|O_TRUNC,0666);
                CHECK_AND_EXIT((out!=-1));
                err = open(fileName, O_RDWR|O_CREAT|O_TRUNC,0666);
                CHECK_AND_EXIT((err!=-1));
                outBkp = dup(fileno(stdout));
                errBkp = dup(fileno(stderr));
                ret = dup2(out, fileno(stdout));
                CHECK_AND_EXIT((ret!=-1));
                ret = dup2(err, fileno(stderr));
                CHECK_AND_EXIT((ret!=-1));
            }
            pid = fork();
            if (pid == 0) { // child
                ret = execv(cmd, args[i]);
                CHECK_AND_EXIT((ret!=-1));
                exit(0);
            } else if (pid < 0) { // fork failed
                ERR_MSG;
                if (fileName != NULL) {
                    // Now that the file descriptors are passed from parent
                    // to child, restore the parent to its original fd's
                    // and continue with the next commands
                    ret = fflush(stdout);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(out);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = fflush(stderr);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(err);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = dup2(outBkp, fileno(stdout));
                    CHECK_AND_EXIT((ret!=-1));
                    ret = dup2(errBkp, fileno(stderr));
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(outBkp);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(errBkp);
                    CHECK_AND_EXIT((ret!=-1));
                    free(fileName);
                }
            } else { // parent
                pidList[i] = pid;
                if (fileName != NULL) {
                    // Now that the file descriptors are passed from parent
                    // to child, restore the parent to its original fd's
                    // and continue with the next commands
                    ret = fflush(stdout);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(out);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = fflush(stderr);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(err);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = dup2(outBkp, fileno(stdout));
                    CHECK_AND_EXIT((ret!=-1));
                    ret = dup2(errBkp, fileno(stderr));
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(outBkp);
                    CHECK_AND_EXIT((ret!=-1));
                    ret = close(errBkp);
                    CHECK_AND_EXIT((ret!=-1));
                    free(fileName);
                }
            }
            free(cmd);
        }
    }
    // wait for all children to finish
    for (i = 0; i < numCmds; i++) {
        int status;
        if (pidList[i] != 0)
            while (-1 == waitpid(pidList[i], &status, 0));
    }
    if (pidList)
        free(pidList);
}

int main(int argc, char *argv[])
{
    char *bufPtr = NULL, *rdPtr = NULL;
    char *defaultPathArgs[2] = {NULL, NULL};
    char *cmdPtr = NULL, *tok = NULL;
    char **paths = NULL, **cmds = NULL, ***args = NULL;
    size_t bufSz = 0, rdSz = 0, cmdSz = 0, argSz = 0;
    bool isBatch = false;
    bool isRedir = false;
    FILE *fp;
    int i = 0, j = 0, cmd = 0;

    if (argc > 2) {
        ERR_MSG;
        exit(1);
    } else if(argc == 2){
        fp = fopen(argv[1],"r");
        if(fp == NULL){
            // bad batch file
            ERR_MSG;
            exit(1);
        }
        isBatch = true;
    }

    // setup default paths
    defaultPathArgs[0] = "/bin";
    handlePath(defaultPathArgs, &paths);

    // Main loop of the shell
    while(1) {
        cmdSz = CMD_BUF_SZ;
        cmds = malloc(cmdSz * sizeof(char *));
        CHECK_AND_EXIT(cmds);

        for (i = 0; i < cmdSz; i++) {
            cmds[i] = NULL;
        }

        if(isBatch){
            rdSz = getline(&bufPtr, &bufSz, fp);
            if(rdSz == -1)
                break;
        } else {
            fp = NULL;
            printf("%s","wish> ");
            fflush(stdout);
            rdSz = getline(&bufPtr, &bufSz, stdin);
        }

        // Parse and build a list of commands
        rdPtr = bufPtr;
        cmds[0] = rdPtr;
        cmdPtr = strchr(rdPtr, '&');
        cmd = 0;
        while(cmdPtr) {
            *(cmds[cmd]+(cmdPtr-cmds[cmd]))='\0';
            rdPtr = cmdPtr+1;
            cmd++;
            // Increase command size if more commands are present.
            if (cmd >= cmdSz) {
                cmdSz += CMD_BUF_SZ;
                cmds = realloc(cmds, (cmdSz * sizeof(char *)));
                CHECK_AND_EXIT(cmds);
                for (i=cmd; i < cmdSz; i++) {
                    cmds[i] = NULL;
                }
            }
            cmds[cmd] = rdPtr;
            cmdPtr = strchr(rdPtr, '&');
        }

        argSz = cmdSz;
        args = malloc(argSz * sizeof(char *));
        CHECK_AND_EXIT(args);

        for (i = 0; i < argSz; i++) {
            args[i] = malloc(argSz * sizeof(char *));
            CHECK_AND_EXIT(args[i]);
        }
        for (i = 0; i < argSz; i++) {
            for (j = 0; j < argSz; j++) {
                args[i][j] = NULL;
            }
        }

        cmd = 0;
        i = 0;
        j = 0;
        while (cmds[cmd] != NULL) {
            rdPtr = cmds[cmd];
            cmd++;
            isRedir = false;
            tok = NULL;
            cmdPtr = NULL;
            // Parse through each command and build a table of cmds - args
            tok = strsep(&rdPtr, " \n\t");
            while(tok != NULL) {
                if (!strcmp(tok,"")) {
                } else {
                    cmdPtr = strchr(tok, '>');
                    // check multiple redirs or '>' without spaces
                    if (!strcmp(tok, ">") || cmdPtr) {
                        if (isRedir) {
                            ERR_MSG;
                            // cleanup args before returning
                            for (j = 0; j < argSz; j++) {
                                args[i][j] = NULL;
                            }
                            j = 0;
                            break;
                        } else {
                            isRedir = true;
                            if (strcmp(tok, ">") != 0) {
                                *(tok+(cmdPtr-tok))='\0';
                                args[i][j++]=tok;
                                args[i][j++]=">";
                                tok = cmdPtr+1;
                                continue;
                            }
                        }
                    }
                    args[i][j++]=tok;
                }
                tok = strsep(&rdPtr, " \n\t");
            }
            // This is not a valid command, continue to the next one
            if (j==0) continue;
            i++;
            j = 0;
        }
        // If there are no commands, don't do anything
        if (i==0 && j==0) {
        } else {
            handleCmds(&paths, args, i);
        }
        if (bufPtr) {
            free(bufPtr);
            bufPtr = NULL;
            free(cmds);
            for (i = 0; i < argSz; i++) {
                free(args[i]);
            }
            free(args);
        }
    }

    if (paths) {
        i = 0;
        while(paths[i]) {
            free(paths[i]);
            i++;
        }
        free(paths);
    }

    return 0;
}
