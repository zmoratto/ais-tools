#include <stdio.h>

FILE *filen;
unsigned long henten(int from, int size, unsigned char *frame)
{
  int i=0;
  unsigned long tmp = 0;
  for(i=0;i<size;i++)
    {
      tmp |= (frame[from+i]-0x30) << (size-1-i);
    }
  return tmp;
}

void get_data(unsigned char *frame, int antall)
{
  int i,j;

  unsigned char type=henten(0,6,frame);
  printf("MessageID: %d\n",type);
  unsigned long mmsi=henten(8,30,frame);
  printf("MMSI: %09d\n",mmsi);

  unsigned long longitude;
  unsigned long latitude;
  unsigned short sog;
  unsigned short heading, heading2;
  char rateofturn;
  char underway;

  switch(type){
  case 1:
    longitude = henten(38+22,29,frame);
    printf("Longitude: %f\n",(float)longitude/600000);

    latitude = henten(38+22+29,27,frame);
    printf("Latitude: %f\n",(float)latitude/600000);

    heading = henten(38+22+28+28,12,frame);
    printf("Heading: %f\n",(float)heading/10);

    sog = henten(38+11,10,frame);
    printf("Knots: %f\n",(float)sog/10);

    rateofturn = henten(38+2,8,frame);
    printf("Rateofturn: %i\n",rateofturn);

    underway = henten(38,2,frame);
    printf("Underway: %d\n",underway);

    heading2 = henten(38+22+28+28+12,9,frame);
    printf("Heading2: %d\n",heading2);
    break;
  }
  printf("\n");
}

void skrivut(unsigned char *frame, int antall, int nummer)
{
  int i;
  fprintf(filen,"alle.append([",nummer+1);
  for(i=0;i<antall;i++)
    {
      fprintf(filen,"%d",frame[i]-0x30);
      if (i<antall-1) fprintf(filen,",");
    }


  fprintf(filen,"])\n");

}

int main(int argc, char **argv)
{
  FILE *in = fopen(argv[1],"r");
  filen = fopen("utfil","w");
  unsigned char frame[250];
  unsigned char tmp;
  int antall=0;
  int baater=0;
  while(1){
    do
      {
        if(fread(&tmp,1,1,in) == 0) goto slutt;

        frame[antall] = tmp;


        antall++;
      } while (tmp != 10);
    skrivut(frame,antall-1,baater);
    get_data(frame, antall);
    antall = 0;
    baater++;
  }
 slutt:
  fclose(in);
  fclose(filen);
  printf("Antall totalt: %d\n",baater);
}
