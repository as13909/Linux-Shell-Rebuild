#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

//Sources:
//https://www.youtube.com/watch?v=5fnVr-zH-SE
//https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r
//https://www.baeldung.com/linux/exec-functions
//https://cplusplus.com/reference/cstdio/freopen/
//https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
//https://stackoverflow.com/questions/14543443/in-c-how-do-you-redirect-stdin-stdout-stderr-to-files-when-making-an-execvp-or
//https://stackoverflow.com/questions/13477281/initializing-an-array-of-ints
//https://www.geeksforgeeks.org/pass-the-value-from-child-process-to-parent-process/
//https://www.delftstack.com/howto/c/use-mmap-function-to-write-to-the-memory-in-c/
//https://www.youtube.com/watch?v=VzCawLzITh0&t=565s
int countPipes(char* in){
    int count =1;
    for(int i = 0; i<(int)strlen(in);i++)if(in[i]=='|')count++;
    return count;
}
char** formatMultiDirect(char* input){
    char* use = malloc(strlen(input)*sizeof(char));
    char* rest;
    strcpy(use,input);
    char* last = malloc(1+sizeof(char)*2);
    for(int i = 0; i < (int)strlen(input)-1;i++){
        if(input[i]=='>'&&input[i+1]=='>'){
            i++;
            last =">>";
        }
        else if(input[i]=='>')last=">";
        else if(input[i]=='<')last="<";
    }
    char** argAR = malloc(sizeof(char*)*4);
    strtok_r(use,last,&rest);
    use[strlen(use)-1]='\0';
    if(strcmp(last,">>")==0)rest=strtok(rest," >");
    else rest=strtok(rest," ");
    
    argAR[0]=use;
    argAR[1]=rest;
    argAR[2]=last;
    return argAR;
    free(use);
    free(argAR);
    free(last);

}
int checkMultiDirectional(char* input){
    if(strstr(input,">")!=NULL && strstr(input,"<")!=NULL)return -1;
    else if(strstr(input,">>")!=NULL && strstr(input,"<")!=NULL)return -1;
    else if(strstr(input,">>")!=NULL && strstr(input," > ")!=NULL)return -2;
    else return 0;

    //0 if not multidirectonal -1 if it is -2 if it is multi-direct but invalid
}

char** argForm(char* input){
    int countArgs=0;
    for(int i = 0; i<(int)strlen(input);i++)if(input[i]==' ')countArgs++;
    char* use = malloc(strlen(input)*sizeof(char));
    strcpy(use,input);
    char** argsAR = (char**)malloc(countArgs*sizeof(char*)+1);
    countArgs=0;
    char* ex = malloc(strlen(input)*sizeof(char));
    ex = strtok(use, " ");
 
    while (ex != NULL) {
        ex[strlen(ex)]='\0';
        argsAR[countArgs]=malloc(strlen(ex)*sizeof(char)+1);
        argsAR[countArgs]=ex;
        countArgs++;
        ex = strtok(NULL, " ");
    }
    argsAR[countArgs]=NULL;
    return argsAR;
    free(use);
    free(argsAR);
    free(ex);
}
char* getFormatedDir(){
    char cwd[PATH_MAX];
    char* curWD =getcwd(cwd,sizeof(cwd));
    char* comPromp;
    int count=0,i=strlen(curWD);
    
    while(curWD[i]!='/'){
        count++;
        i--;      
    }
    comPromp= malloc(12+count);
    strcpy(comPromp,"[nyush ");
    char* parse;
    char* use;
    if(strcmp(curWD,"/")==0){
        strcat(comPromp, "/");
    }
    else{
        parse=strtok(curWD,"/");
        while(parse!=NULL){
            use = malloc(strlen(parse)*sizeof(char));
            strcpy(use,parse);
            parse=strtok(NULL,"/");
            
        }
        strcat(comPromp,use);
    }
    
    strcat(comPromp,"]$ ");
    return comPromp;
    free(comPromp);
    free(use);
}
char** formCom(char* input){
    char* te="";
    int x=0;
    te=malloc(strlen(input)*sizeof(char));
    strcpy(te,input);
    char** ret = malloc(3*sizeof(char*));
    char* bi=malloc(sizeof(char)*strlen(input));
    char* temp="";
    if(strstr(input, ">>")!=NULL){
        bi = strtok(te,">>");
        bi[strlen(bi)]='\0';
        ret[0]=malloc(strlen(bi)*sizeof(char));
        ret[0]=bi;
        temp=malloc((strlen(input)-strlen(ret[0]))*sizeof(char));
        while (bi != NULL){
            if(x!=0){
                if(x!=1)strcat(temp," ");
                strcat(temp,bi);
            }
            x++;
            bi = strtok(NULL, ">> ");
        }
    }
    else if(strchr(input,'>')!=NULL){
        bi = strtok(te,">");
        bi[strlen(bi)-1]='\0';
        ret[0]=bi;
        temp=malloc((strlen(input)-strlen(ret[0]))*sizeof(char));
        while (bi != NULL){
            if(x!=0){
                if(x!=1)strcat(temp," ");
                strcat(temp,bi);
            }
            x++;
            bi = strtok(NULL, " > ");
        }
    }
    else if(strchr(input,'<')!=NULL){
        bi = strtok(te,"<");
        bi[strlen(bi)-1]='\0';
        ret[0]=bi;
        temp=malloc((strlen(input)-strlen(ret[0]))*sizeof(char));
        while (bi != NULL){
            if(x!=0){
                if(x!=1)strcat(temp," ");
                strcat(temp,bi);
            }
            x++;
            bi = strtok(NULL, " < ");
        }
    }
    else{
        bi = strtok(te," ");
        ret[0]=bi;
        temp=malloc((strlen(input)-strlen(ret[0]))*sizeof(char));
        while (bi != NULL){
            if(x!=0){
                if(x!=1)strcat(temp," ");
                strcat(temp,bi);
            }
            x++;
            bi = strtok(NULL, " ");
            
        }
    }
    ret[1]=malloc(strlen(temp)*sizeof(char));
    ret[1]=temp;
    ret[2]=NULL;
    return(ret);
    free(ret);
    free(bi);
    free(te);
    free(temp);
}
int checkMultiArg(char* ar){
    int count =0;
    for(int i=0;i<(int)strlen(ar);i++){
        if(isspace(ar[i])!=0)count++;
    }
    return count;
    //if multi arg returns -1 if just one arg returns 0
}

int checkNothing(char* in){
    for(int i = 0;i<(int)strlen(in);i++)if(isspace(in[i])==0)return 0;
    return -1;
}
int myVAR = 0;
void handle_sigtstp(int sig){
    myVAR = sig + 1 -1;
    int x =0;
    sig = sig + x;
    //HEY! I'm using sig... sorta :)
}
int checkBadPipe(char* in){
    int check = 0;
    for(int i =0;i<(int)strlen(in);i++){
        if(in[i]=='|'&&check==0)return -1;
        else if(in[i]=='|')check=0;
        else if(isspace(in[i])!=0);
        else check++;
    }
    return 0;
}
char** pipeForm(char* input){
    int countPipes=1;
    for(int i = 0; i<(int)strlen(input);i++){
        if(input[i]=='|')countPipes++;
    }
    if(countPipes==1){
        char** pipeAR = malloc(3*sizeof(char*));
        pipeAR[0]=input;
        pipeAR[1]=NULL;
        return pipeAR;
    }
    char* use = malloc(strlen(input)*sizeof(char));
    strcpy(use,input);
    char** pipeAR = malloc(2+countPipes*sizeof(char*));
    int c = 0;
    char* ex = strtok(use, "|");
 
    while (ex != NULL) {
        ex[strlen(ex)]='\0';
        pipeAR[c]=ex;
        c++;
        ex = strtok(NULL, "|");
    }
    return pipeAR;
    free(use);
    free(pipeAR);
}
int main(){
    //SHARED MEMORY!
    
    //first element represents which job number we're on!
    fflush(stdout);
    char* input = "";
    size_t size = 0;
    char* dir = malloc(strlen(getFormatedDir())*sizeof(char));
    dir = getFormatedDir();
    char** jobNames=malloc(100*sizeof(char*));
    int pidAR[100];
    int jobNum=0;
    int bool =0;
    
    // char cwd[PATH_MAX];
    // char* baseDIR=malloc(sizeof(cwd));
    // baseDIR =getcwd(cwd,sizeof(cwd));

    while(bool==0){
        //SIGNAL HANDLING
        signal(SIGQUIT,SIG_IGN);
        signal(SIGINT,SIG_IGN);
        signal(SIGTSTP,SIG_IGN);
        
        dir = getFormatedDir();

        //Prompt!
        printf("%s",dir);
        
        fflush(stdout);
        //input!
        if(getline(&input, &size, stdin)==-1)exit(0);
        input[strcspn(input, "\r\n")] = 0;
        fflush(stdout);
        
        //Check to make sure user isn't inputting just an enter
        //Keeps printing prompt if input = ""
        if(strcmp(input,"")==0 || checkNothing(input)==-1);
        else{
            //pipes!
            int numPipes = countPipes(input);
            int pids[numPipes+1];
            int pipes[numPipes+1][2];
            char** pipeAR=(char**)malloc(sizeof(char*)*numPipes+2);
            pipeAR=pipeForm(input);

            char* arg2="";
            char* arg1="";
            int stat;
            int pidRun;
            
            
            int fd[2];
            if(pipe(fd)==-1){
                fprintf(stderr,"error");
                return 0;
            }
            for(int q = 0; q<numPipes;q++){
                
                input = malloc(strlen(pipeAR[q])*sizeof(char));
                input=pipeAR[q];
                
                if(input[strlen(input)-1]==' ')input[strlen(input)-1]='\0';
                if(input[0]==' ')input=strtok(input," ");
                printf("INPUT |%s|\n",input);

                arg2 = malloc(strlen(formCom(input)[1])*sizeof(char*)+1);
                strcpy(arg2,formCom(input)[1]);
                arg1 = malloc(strlen(formCom(input)[0])*sizeof(char*));
                strcpy(arg1,formCom(input)[0]);
                char* com =malloc(strlen(input)*sizeof(char));

                strcpy(com, input);
                strtok(com, " ");
                com[strcspn(com, "\r\n")] = 0;
                
                arg2[strcspn(arg2, "\r\n")] = 0;
                if(arg2[strlen(arg2)-1]=='!')arg2[strlen(arg2)-1]='\0'; 


                //FORKING!
                pids[q]=fork();
                if(pids[q]==-1) return -1;
                else if(pids[q]==0){
                    //child process
                    for(int j=0;j<numPipes;j++){
                        if(q!=j)close(pipes[j][0]);
                        if(q+1!=j)close(pipes[j][1]);
                    }
                    dup2(pipes[q][0],STDIN_FILENO);
                    dup2(pipes[q+1][1],STDOUT_FILENO);
                    close(pipes[q+1][1]);
                    close(pipes[q][0]);
                    
                    if(strcmp(input,"exit")==0){
                        exit(0);
                    }
                    if(checkMultiDirectional(input)!=0){
                        if(checkMultiDirectional(input)==-2)fprintf(stderr,"Error: invalid command\n");
                        else{
                            //add in more file creation elements!
                            char* a3="EMPTY";
                            char* a1 = malloc(strlen(formCom(formatMultiDirect(input)[0])[0])*sizeof(char));
                            char* a2 = malloc(strlen(formCom(formatMultiDirect(input)[0])[1])*sizeof(char));
                            char* fl = malloc(strlen(formatMultiDirect(input)[1])*sizeof(char));
                            fl = formatMultiDirect(input)[1];
                            a1 = formCom(formatMultiDirect(input)[0])[0];
                            a2 = formCom(formatMultiDirect(input)[0])[1];
                            if(strstr(a1,"cat -")!=NULL){
                                char* temp = malloc(strlen(a1)*sizeof(char));
                                char* use1=malloc(strlen(a1)*sizeof(char));
                                strcpy(temp,a1);
                                use1 = strtok(temp, " ");
                                // Keep printing tokens while one of the
                                // delimiters present in str[].
                                a3 = malloc(strlen(a1)*sizeof(char));
                                while (use1 != NULL) {
                                    strcpy(a3,use1);
                                    use1 = strtok(NULL, " ");
                                }
                                
                                
                                if(access(a3,F_OK|W_OK)==-1){
                                    fprintf(stderr,"Error: invalid command\n");
                                    exit(0); 
                                }
                                //free(use1);
                                free(temp);
                            }
                            if(fl[strlen(fl)-1]=='!')fl[strlen(fl)-1]='\0';
                            if(strstr(a1," > ")!=NULL){
                                if(access(a2,F_OK|W_OK)==-1){
                                    fprintf(stderr,"Error: invalid command\n");
                                    exit(0);
                                }
                            }
                            if(strcmp(formatMultiDirect(input)[2],">")!=0){
                                if(access(fl,F_OK|W_OK)==-1){
                                    fprintf(stderr,"Error: invalid command\n");
                                    exit(0);
                                }
                            }
                            if(strcmp(a1,"cat")!=0||strcmp(a3,"EMPTY")==0){
                                
                                if(strcmp(formatMultiDirect(input)[2],">")==0 || strcmp(formatMultiDirect(input)[2],">>")==0){
                                    
                                    FILE *file1,*file2,*file3;
                                    char c;
                                    if(strcmp(a3,"EMPTY")!=0){
                                        file3= freopen(a3,"r",stdin);
                                        c=fgetc(file3);
                                    }
                                    if(strcmp(formatMultiDirect(input)[2],">>")==0)file2 = freopen(fl,"a+",stdout);
                                    else file2 = freopen(fl,"w+",stdout);
                                    while(c!=EOF){
                                        fprintf(file2,"%c",c);
                                        c=fgetc(file3);
                                    }
                                    if(strcmp(a3,"EMPTY")!=0){
                                        fclose(file3);
                                        free(a3);
                                    }
                                    file1 = freopen(a2,"r",stdin);
                                    c=fgetc(file1);
                                    
                                    while(c!=EOF){
                                        fprintf(file2,"%c",c);
                                        c=fgetc(file1);
                                    } 
                                    
                                    fclose(file1);
                                    fclose(file2);
                                }
                                else if(strcmp(formatMultiDirect(input)[2],"<")==0){
                                    FILE *file1,*file2,*file3;
                                    char c;
                                    if(strstr(formatMultiDirect(input)[0],">>")!=NULL){
                                        file2=freopen(a2,"a+",stdout);
                                    }
                                    else file2=freopen(a2,"w+",stdout);
                                    if(strcmp(a3,"EMPTY")!=0){
                                        file3= freopen(a3,"r",stdin);
                                        c=fgetc(file3);
                                    }
                                    while(c!=EOF){
                                        fprintf(file2,"%c",c);
                                        c=fgetc(file3);
                                    }
                                    if(strcmp(a3,"EMPTY")!=0){
                                        fclose(file3);
                                        free(a3);
                                    }
                                    file1 = freopen(fl,"r",stdin);
                                    c=fgetc(file1);
                                    while(c!=EOF){
                                        fprintf(file2,"%c",c);
                                        c=fgetc(file1);
                                    } 
                                    fclose(file1);
                                    fclose(file2);
                                }
                            
                            }
                            free(a1);
                            free(a2);
                        }
                        exit(0);
                    }
                    else if(strstr(input,"|")!=NULL){
                        if(checkBadPipe(input)==-1)fprintf(stderr,"Error: invalid command\n");
                        exit(0);
                    }
                    else if(strstr(input,">>")!=NULL){
                        if(strcmp(arg2,"")==0)fprintf(stderr,"Error: invalid command\n");           
                        else if(access(arg2,F_OK | W_OK)!=-1){
                            FILE *file = freopen(arg2,"a+",stdout);
                            if(strcmp(arg1,"cat")==0){
                                int loop =0;
                                char* Catinput = "";
                                size_t Catsize = 0;
                                signal(SIGQUIT,SIG_DFL);
                                signal(SIGINT,SIG_DFL);
                                signal(SIGTSTP,SIG_DFL);
                                while(loop==0){
                                    fflush(stdout);
                                    getline(&Catinput,&Catsize,stdin);
                                    fprintf(file,Catinput);
                                }
                            }
                            else{
                                char *file2 = malloc(sizeof(char)*strlen(argForm(arg1)[0])+10);
                                char* temp="";
                                strcpy(file2,temp);
                                strcat(file2,argForm(arg1)[0]);
                                if(execv(file2,argForm(arg1))==-1)fprintf(stderr,"Error: invalid program\n");
                                free(file2);
                            }
                            fclose(file);
                        }
                        else fprintf(stderr,"Error: invalid file\n");
                        exit(0);
                    }
                    else if(strchr(input,'>')!=NULL){
                        if(strcmp(arg2,"")==0)fprintf(stderr,"Error: invalid command\n");    
                        else{
                            static int fd;
                            static fpos_t pos;
                            fgetpos(stdout,&pos);
                            fd = dup(fileno(stdout));
                            FILE *file = freopen(arg2,"w+",stdout);
                            if(access(arg2,F_OK | W_OK | R_OK)==-1){
                            }
                            if(strcmp(arg1,"cat")==0){
                                int loop =0;
                                char* Catinput = "";
                                size_t Catsize = 0;
                                signal(SIGQUIT,SIG_DFL);
                                signal(SIGINT,SIG_DFL);
                                signal(SIGTSTP,SIG_DFL);
                                while(loop==0){
                                    fflush(stdout);
                                    getline(&Catinput,&Catsize,stdin);
                                    fprintf(file,Catinput);
                                }
                            }
                            else{
                                fflush(stdout);
                                char *file2 = malloc(sizeof(char)*strlen(argForm(arg1)[0])+10);
                                char* temp="/usr/bin/";
                                strcpy(file2,temp);
                                strcat(file2,argForm(arg1)[0]);
                                if(execv(file2,argForm(arg1))==-1)fprintf(stderr,"Error: invalid program\n");
                                free(file2);
                            }
                            close(fd);
                            fclose(file);
                        }
                        exit(0);
                    }
                    else if(strchr(input,'<')!=NULL){
                        if(strcmp(arg2,"")==0)fprintf(stderr,"Error: invalid command\n");   
                        else{
                            FILE *file;
                            char c;
                            if(access(arg2, F_OK | R_OK)!=-1){
                                file = freopen(arg2,"r",stdin);
                                if(strcmp(arg1,"cat")!=0){
                                    char *file2 = malloc(sizeof(char)*strlen(argForm(arg1)[0])+10);
                                    char* temp="/usr/bin/";
                                    strcpy(file2,temp);
                                    strcat(file2,argForm(arg1)[0]);
                                    if(execv(file2,argForm(arg1))==-1)fprintf(stderr,"Error: invalid command\n");
                                }
                                c=fgetc(file);
                                while(c!=EOF){
                                    printf("%c",c);
                                    c=fgetc(file);
                                }
                                fclose(file);                      
                            }
                            else fprintf(stderr,"Error: invalid file\n");
                            
                        }
                        exit(0);

                    }
                    else if(strcmp(com,"ls")==0){
                        if(checkMultiArg(input)>1)fprintf(stderr,"Error: invalid command\n");
                        else{
                            char *file="ls";
                            char *aw = arg2;
                            if(strcmp(input,"ls")==0)aw="-c";
                            if(execlp(file,file,aw,NULL)==-1)fprintf(stderr,"Error: invalid command\n");
                        }
                        exit(0);
                    }
                    else if(strcmp(arg1,"cd")==0){  
                        exit(0);
                        
                    }
                    else if(input[0]=='.'|| input[0]=='/' || strstr(input,"/")!=NULL){

                        if(strstr(input,"-")!=NULL){
                            char *file = malloc(strlen(arg1)*sizeof(char));
                            file = arg1;
                            if(execl(file, file, arg2, NULL)==-1)fprintf(stderr,"Error: invalid program\n");
                            free(file);
                        }
                        else{
                            if(checkMultiArg(input)>=1){
                                signal(SIGQUIT,SIG_DFL);
                                signal(SIGINT,SIG_DFL);
                                signal(SIGTSTP,SIG_DFL);
            
                                char *file = malloc(strlen(arg1)*sizeof(char));
                                file = arg1;
                                if(input[0]=='/'){
                                    if(execv(file,argForm(input))==-1)fprintf(stderr,"Error: invalid program\n");
                                }
                                else{
                                    if(execvp(file, argForm(input))==-1)fprintf(stderr,"Error: invalid program\n");
                                }
                                free(file);
                            }
                            else{
                                signal(SIGQUIT,SIG_DFL);
                                signal(SIGINT,SIG_DFL);
                                signal(SIGTSTP,SIG_DFL);
                                char *file = input;
                                char *const args[] = {file, NULL};
                                if(input[0]=='/'){
                                    if(execv(file,args)==-1)fprintf(stderr,"Error: invalid program\n");
                                }
                                else{
                                    if(execvp(file, args)==-1)fprintf(stderr,"Error: invalid program\n");
                                } 
                            }
                        }
                        exit(0);
                    }
                    else if(strstr(input,"cat")!=NULL && strstr(input,"|")==NULL){
                        if(strcmp(input,"cat")!=0){
                            FILE *file;
                            char c;
                            if(access(arg2, F_OK | R_OK | W_OK)!=-1){
                                file = freopen(arg2,"r",stdin);
                                c=fgetc(file);
                                while(c!=EOF){
                                    printf("%c",c);
                                    c=fgetc(file);
                                }                      
                            }
                            else fprintf(stderr,"Error: invalid file\n");
                        }
                        else{
                            int loop =0;
                            char* Catinput = "";
                            size_t Catsize = 0;
                            signal(SIGQUIT,SIG_DFL);
                            signal(SIGINT,SIG_DFL);
                            signal(SIGTSTP,SIG_DFL);
                            while(loop==0){
                                fflush(stdout);
                                getline(&Catinput,&Catsize,stdin);
                                printf("%s",Catinput);
                            }
                        }
                        exit(0);
                    }
                    else if(strcmp(input,"jobs")==0){
                        for(int i = 0;i<jobNum;i++){
                            printf("[%d] %s\n",i+1,jobNames[i]);
                        }
                        exit(0);
                    }
                    else if(strcmp(arg1,"fg")==0){
                        if(checkMultiArg(input)>2)fprintf(stderr,"Error: invalid job\n");
                        else if(jobNum==0)fprintf(stderr,"Error: invalid job\n");
                        else if(atoi(arg2)<1||atoi(arg2)>jobNum)fprintf(stderr,"Error: invalid job\n");
                        else{
                            pid_t help= pidAR[atoi(arg2)-1];
                            kill(help,SIGCONT);
                        }
                        exit(0);

                    }
                    else if(strcmp(arg1,"killall")==0){
                        exit(0);
                    }
                    else{
                        //Unknown command/weird command
                        char* file ="";
                        file = malloc(8+strlen(arg1)*sizeof(char));
                        strcpy(file,"/usr/bin/");
                        strcat(file,arg1);
                        file[strcspn(file, "\r\n")] = 0;
                        char* args[] ={file, NULL};
                        if(strcmp(arg2,"")!=0){
                            // char** ar = malloc(sizeof(argForm(input)));
                            // ar = argForm(input);
                            // ar[0]=malloc(strlen(file)*sizeof(char));
                            // strcpy(ar[0],);
                            char *file2 = arg1;
                            execvp(file2, argForm(input));
                        }
                        else if(execv(file, args)==-1&&strcmp(arg2,"")==0)fprintf(stderr,"Error: invalid program\n");
                        else fprintf(stderr,"Error: invalid command\n");
                        free(file);
                        exit(0);
                    }
                    
                   
                    return 0;
                }
                    
            }
            for(int q = 0; q<numPipes;q++){
                waitpid(pids[q],&stat,WUNTRACED | WCONTINUED | WSTOPPED);
            
                if(strcmp(input,"exit")==0 || strcmp(arg1,"exit")==0){
                    if(checkMultiArg(input)>0)fprintf(stderr,"Error: invalid command\n");
                    else if(jobNum>0)fprintf(stderr,"Error: there are suspended jobs\n");
                    else{
                        return 0;
                    }
                }
                if(strcmp(arg1,"fg")==0){
                    if(checkMultiArg(input)>2);
                    else if(jobNum==0);
                    else if(atoi(arg2)<1||atoi(arg2)>jobNum);
                    else{
                        waitpid(pidAR[atoi(arg2)-1],&stat,WUNTRACED | WSTOPPED);
                        pidRun=pidAR[atoi(arg2)-1];
                        
                    }
                }
                if(strcmp(arg1,"cd")==0){
                    if(checkMultiArg(input)>1)fprintf(stderr,"Error: invalid command\n");
                    else if(checkNothing(arg2)!=0)fprintf(stderr,"Error: invalid command\n");
                    else{
                        if(chdir(arg2)==-1)fprintf(stderr,"Error: invalid directory\n");
                    }
                }
                if(WIFSTOPPED(stat)){
                    //Entering into loop figure out why
                    //JobNums needs to be udated to reflect current jobNumber
                    //trace jobNums through program and figure out where its gettinng updated
                    if(strcmp(arg1,"fg")==0 && pidAR[atoi(arg2)-1]==pidRun){
                        
                        int c =atoi(arg2)-1;
                        char* t = malloc(strlen(jobNames[atoi(arg2)-1])*sizeof(char));
                        strcpy(t,jobNames[atoi(arg2)-1]);
                        if(jobNum>1 && atoi(arg2)!=jobNum){
                            for(int i = atoi(arg2)-1; i<jobNum-1;i++){
                                c++;
                                jobNames[i]=realloc(jobNames[i],strlen(jobNames[i+1])*sizeof(char));
                                strcpy(jobNames[i],jobNames[i+1]);
                                pidAR[i]=pidAR[i+1];
                            }
                            pidAR[c]=pidRun;
                            jobNames[c]=malloc(strlen(t)*sizeof(char));
                            strcpy(jobNames[c],t);
                        }
                        free(t);
                    }
                    else if(strcmp(arg1,"fg")==0){
                        if(atoi(arg2)==jobNum){
                            free(jobNames[jobNum-1]);
                            jobNames[jobNum-1]=NULL;
                            pidAR[jobNum-1]=0;
                        }
                        else{
                            for(int i = atoi(arg2)-1; i<jobNum-1;i++){
                                jobNames[i]=realloc(jobNames[i],strlen(jobNames[i+1])*sizeof(char));
                                strcpy(jobNames[i],jobNames[i+1]);
                                pidAR[i]=pidAR[i+1];
                            }
                        }
                        jobNum--;
                    }
                    else{
                        jobNames[jobNum]=malloc(strlen(input)*sizeof(char));
                        strcpy(jobNames[jobNum],input);
                        pidAR[jobNum]=pids[q];
                        jobNum++;
                    }                        
                }
                else if(strcmp(arg1,"fg")==0){
                    if(jobNames[atoi(arg2)-1][0]=='.'|| jobNames[atoi(arg2)-1][0]=='/' || strstr(jobNames[atoi(arg2)-1],"/")!=NULL){ 
                        for(int i = atoi(arg2)-1; i<jobNum-1;i++){
                            jobNames[i]=realloc(jobNames[i],strlen(jobNames[i+1])*sizeof(char));
                            strcpy(jobNames[i],jobNames[i+1]);
                            pidAR[i]=pidAR[i+1];
                        }
                        jobNum--;
                    }
                }                
        
            }
        }
        //Don't Change Below!!!!
    }
    return 0;
}
    
//To do!
    //Steps 8-9 on lab2 PDF (i.e. pipes)
    //Continue to cite sources
    //Have fun :)
