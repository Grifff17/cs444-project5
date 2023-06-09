#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "mkfs.h"
#include "inode.h"
#include "block.h"
#include "free.h"
#include "image.h"
#include "pack.h"
#include "directory.h"

void ls(void)
{
    struct directory *dir;
    struct directory_entry ent;

    dir = directory_open(0);

    while (directory_get(dir, &ent) != -1)
        printf("%d %s\n", ent.inode_num, ent.name);

    directory_close(dir);
}