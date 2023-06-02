#define _XOPEN_SOURCE 600 /* Get nftw() */
#include <ftw.h>
#include <grp.h>
#include <pwd.h>

#include <sys/types.h>    /* Type definitions used by many programs */
#include <stdio.h>        /* Standard I/O functions */
#include <stdlib.h>       /* Prototypes of commonly used library functions,
                             plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>       /* Prototypes for many system calls */
#include <errno.h>        /* Declares errno and defines error constants */
#include <string.h>       /* Commonly used string-handling functions */


// A function that returns a string of the group name of a file
char *get_file_group(struct stat *st)
{
    // Use getgrgid to get the group name
    struct group *grp = getgrgid(st->st_gid);
    if (grp == NULL)
    {
        return NULL;
    }

    // Return the group name
    return grp->gr_name;
}

// A function that returns a string of the user name of a file
char *get_file_user(struct stat *st)
{
    // Use getpwuid to get the user name
    struct passwd *pwd = getpwuid(st->st_uid);
    if (pwd == NULL)
    {
        return NULL;
    }

    // Return the user name
    return pwd->pw_name;
}

/// A function that converts a file mode to a string of rwx
char *permissions_to_unix_name(mode_t mode)
{
    // An array of strings that represent the permissions
    char *perm_strings[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};

    // A static buffer to store the result
    static char result[11];

    // // The first character indicates the file type
    result[0] = "";//S_ISDIR(mode) ? 'd' : '-';

    // The next nine characters indicate the permissions for user, group and others
    for (int i = 0; i < 3; i++)
    {
        // Get the three bits that correspond to the permissions
        int perm_bits = (mode >> ((2 - i) * 3)) & 0x7;

        // Copy the appropriate string from the array
        char *perm_string = perm_strings[perm_bits];
        for (int j = 0; j < 3; j++)
        {
            result[i * 3 + j ] = perm_string[j];
        }
    }

    // The last character is a null terminator
    result[10] = '\0';

    // Return the buffer
    return result;
}


static int              /* Callback function called by ftw() */
dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb)
{
    
    for (int i = 0; i < ftwb->level; i++)
    {
        if(i == 0)
        printf("   ");
        else
        printf("│   ");
    }
    if (type == FTW_NS) {                  /* Could not stat() file */
        printf("?");
    } else {
        switch (sbuf->st_mode & S_IFMT) {  /* Print file type */
        case S_IFREG:  printf("├── "); printf(" [-");  break;
        case S_IFDIR:  printf("└── ");  printf(" [d"); break;
        case S_IFCHR:  printf(" [c"); break;
        case S_IFBLK:  printf(" [b"); break;
        case S_IFLNK:  printf(" [l"); break;
        case S_IFIFO:  printf(" [p"); break;
        case S_IFSOCK: printf(" [s"); break;
        default:       printf(" [?"); break; /* Should never happen (on Linux) */
        }
    }


    printf("%s", permissions_to_unix_name(sbuf->st_mode));

    printf(" %s", get_file_group(sbuf));
    printf(" %s", get_file_user(sbuf));

    printf(" %ld", sbuf->st_size);

	
    printf("] ");
    printf("%s\n",  &pathname[ftwb->base]);     /* Print basename */
    return 0;                                   /* Tell nftw() to continue */
}

int
main(int argc, char *argv[])
{
    int flags = 0;
   
    if (nftw(".", dirTree, 10, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
