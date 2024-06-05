// C program to print
// inverted pyramid
// pattern
#include <stdio.h>
#include <unistd.h>
 
// Driver code
int main()
{
   int rows = 8, i, j, space;
 
   for (i = rows; i >= 1; --i) 
   {
       // Loop to print the blank space
       for (space = 0; 
            space < rows - i; ++space)
           write(STDOUT_FILENO, "  ", 2);
      
       // Loop to print the half of 
       // the star triangle
       for (j = i; j <= 2 * i - 1; ++j)
           write(STDOUT_FILENO, "* ", 2);
      
       // Loop to print the rest of 
       // the star triangle
       for (j = 0; j < i - 1; ++j)
           write(STDOUT_FILENO, "* ", 2);
       write(STDOUT_FILENO, "\n", 1);
       //sleep(1);
   }
 
   return 0;
}
