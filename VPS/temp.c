#include <stdio.h>
#include <stdlib.h>

#define FILENAME_MAX    (256)
#define WORD_MAX        (256)
#define PARAMETER_MAX   (4)
#define PROCESS_NUM_MAX (20)
#define PROCESS_NUM_MIN (1)
#define CHAR_SPACE      (' ')
#define CHAR_TAB        ('\t')
#define CHAR_ENTER      ('\n')

/*
    Usage: ./WordCounter [FILE NAME] [TARGET WORD] [PROCESS NUM]
    ex: ./WordCounter filename.txt target 3
*/
int main(int argc, char **argv)
{
    char strFName[FILENAME_MAX] = {};           /* FILE NAME */
    char strTargetWord[WORD_MAX] = {};          /* TARGET WORD */
    char strTempWord[WORD_MAX] = {};            /* Word from file */
    char cTemp = 0;
    unsigned int unProcessNum = 0;              /* PROCESS NUM */
    unsigned int unIndex = 0;
    unsigned int unWordIndex = 0;
    int nPid = 0;
    FILE *fp = NULL;
    long fsize = 0;
    long lHandlSize = 0;                        /* Handling file size for each process */
    long lCharCnt = 0;                          /* Number of characters read from file  */
    long lTargetCnt = 0;                        /* Number of target word of child process  */
    long lRead = 0;                             /* Value that read from pipe  */
    long lRstCnt = 0;                           /* The result  */

    printf("%d, %s, %s, %d\n", argc, argv[0], argv[1], __LINE__);

    /********************************************/
    /* Parameter Check                          */
    /********************************************/
    if( PARAMETER_MAX != argc ){
        printf("[ERROR] %d:Parameter error\n", __LINE__);
        printf("Usage: ./WordCounter [FILE NAME] [TARGET WORD] [PROCESS NUM]");
        return -1;
    }

    unProcessNum=atoi(argv[3]);
    if( PROCESS_NUM_MAX < unProcessNum ){
        printf("unProcessNum is bigger than the max value. Reset unProcessNum = PROCESS_NUM_MAX");
        unProcessNum = PROCESS_NUM_MAX;
    }
    if( PROCESS_NUM_MIN > unProcessNum ){
        printf("unProcessNum is smaller than the min value. Reset unProcessNum = PROCESS_NUM_MIN");
        unProcessNum = PROCESS_NUM_MIN;
    }

    /********************************************/
    /* Open file                                */
    /********************************************/
    fp = fopen(argv[1], "r");
    if( NULL == fp ){
        printf("[ERROR] %d:fopen() failed\n", __LINE__);
        printf("Usage: ./WordCounter [FILE NAME] [TARGET WORD] [PROCESS NUM]");
        return -1;
    }

    if( 0 != fseek( fp, 0, SEEK_END ) ){
        printf("[ERROR] %d:fseek() failed\n", __LINE__);
        fclose(fp);
        return -1;
    }
    fsize = ftell(fp);
    if( 0 > fsize ){
        printf("[ERROR] %d:ftell() failed\n", __LINE__);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    /********************************************/
    /* Multi-Process word count                 */
    /********************************************/
    int nPip[unProcessNum][2];

    /* 1 ~ unProcessNum-1 Process dill with file size [lHandlSize] */
    /* The last process dill with file size [lHandlSize~lHandlSize-1] */
    lHandlSize = fsize/unProcessNum;

    for( unIndex = 0; unIndex < unProcessNum; unIndex++ ){
        if( 0 != pipe(nPip[unIndex]) ){
            printf("[ERROR] %d:Pipe create failed\n", __LINE__);
            return -1;
        }

        nPid = fork();
        if( 0 > nPid ){
            printf("[ERROR] %d:fork failed\n", __LINE__);
            exit(1);
        }else if( 0 == nPid ){
            printf("[DEBUG] Child process\n");

            fp = fopen( argv[1], "r" );
            if( NULL == fp ){
                printf("[ERROR] %d:fopen() failed\n", __LINE__);
                exit(1);
            }

            if( 0 != fseek( fp, lHandlSize*unIndex, SEEK_SET ) ){
                printf("[ERROR] %d:fseek() failed\n", __LINE__);
                fclose(fp);
                exit(1);
            }

            while( EOF != (cTemp=getc(fp)) ){
                lCharCnt++;
                if( (unIndex < unProcessNum-1) && (lCharCnt >= lHandlSize) ){
                    break;
                }

                if( ( CHAR_SPACE != cTemp ) && ( CHAR_TAB != cTemp ) && ( CHAR_ENTER != cTemp ) )
                {
                    strTempWord[unWordIndex++]=cTemp;
                }else{
                    strTempWord[unWordIndex] = '\0';
                    if( 0 == strcmp(argv[2], strTempWord) ){
                        lTargetCnt++;
                        unWordIndex = 0;
                    }
                }
            }

            fclose(fp);

            close(nPip[unIndex][0]);

            if( -1 == write(nPip[unIndex][1], &lTargetCnt, sizeof(lTargetCnt)) ){
                printf("[ERROR] %d:write() failed\n", __LINE__);
                exit(1);
            }
            close(nPip[unIndex][1]);
            _exit(0);
        }
    }

    while(-1 != wait(NULL))
    {
        ;   /* do nothing */
    }

    for( unIndex = 0; unIndex < unProcessNum; unIndex++ ){
        if( 0 < read(nPip[unIndex][0], &lRead, sizeof(lRead)) ){
            lRstCnt += lRead;
        }

        close(nPip[unIndex][0]);
        close(nPip[unIndex][1]);
    }

    printf("\nThe number of [%s] is: %d ", argv[2], lRstCnt);


    return 0;
}
