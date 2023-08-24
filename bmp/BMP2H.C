#include <stdio.h>


int main(void)
{
    char fntn[100];

    char  outfn[100],infn[100];
    int i,j,k,comp,gc,pn;
    unsigned long int fn;
    FILE *infile,*outfile;
    //clrscr();
    printf("input BMP file name : ");

    scanf("%s",fntn);
    sprintf(outfn,"%s.h",fntn);
    outfile=fopen(outfn,"wb");

    sprintf(infn,"%s.bmp",fntn);
    infile=fopen(infn,"rb");

    fprintf(outfile,"const char %s[] = {",fntn);


    pn=0;
    for ( fn=0; ;fn++ )
    {
	if(gc==EOF) break;
	if (pn==0) {  fprintf(outfile,"\n/*%lu*/",fn); }

	gc = getc(infile);
	fprintf(outfile,"0x%02x, ",gc);

	if(fn<53){if(pn < 15) pn++;  else pn=0;}
	else if(fn<1074){if(pn<3) pn++; else pn=0;}
	else {if(pn<9) pn++; else pn=0;}

    }
    printf("file size = %lu\n" ,fn);
    fprintf(outfile,"\n }; \n ");
    fclose(outfile);
    fclose(infile);
    //getch();
    return 1;
}
