#include <stdio.h>
#include <stdlib.h>
char  *Names[] = { "rax", "rbx", "rcx", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
char  **Namep  = Names;   
   
char  *newname()   
{   
    if( Namep >= &Names[ sizeof(Names)/sizeof(*Names) ] )   
    {   
        fprintf( stderr, "%d: Expression too complex\n", 7 );   
        exit( 1 );   
    }   
   //printf("%s\n", *Namep);
   //printf("newname called %s\n", *Namep);
    return( *Namep++ );   
}   
   
void freename(char* s)     
{   
    if( Namep > Names ){   
    *--Namep = s;
    //printf("freed %s\n", s);
    }
    else   
    fprintf(stderr, "%d: (Internal error) Name stack underflow\n",   
                                7 );   
}

