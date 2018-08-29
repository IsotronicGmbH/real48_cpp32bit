/*                      BPRTEST                      */
/* Demonstrates Borland Pascal Real-Type Conversions */
/*                  by Richard Biffl                 */

#include <stdio.h>
#include "bpreal.c"


char *convmessages[] = {
    "Ok ", "+Un", "-Un", "Ovr", "Inf", "NaN"
};


int main()
/* write and read file of Borland Pascal reals, BPREALS.DAT */
{
    FILE *realfile;
    int i;
    double d;
    real r;

    printf("\nPrompting for 6 values to write to BPREALS.DAT\n");
    realfile = fopen("BPREALS.DAT", "w+b");
    for (i = 1; i <= 6; i++) {
        do {
            fflush(stdin);
            printf("Enter a floating-point value: ");
        } while (scanf("%lf", &d) == 0);
        printf("% 23.16lg      Conversion result: %s\n",
               d, convmessages[double_to_real(d, &r)]);
        fwrite(&r, sizeof(r), 1, realfile);
    }
    rewind(realfile);
    printf("\nReading and converting real values in BPREALS.DAT\n");
    while (fread (&r, sizeof(r), 1, realfile))
        printf ("Real value (converted to double): %23.16lg\n",
                real_to_double(r));
    fclose(realfile);
    return 0;
}
