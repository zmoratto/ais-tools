#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>

#include <iomanip>
#include <mysql/mysql.h>
using namespace std;

ostringstream kmlfil;


static char *headerkml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <kml xmlns=\"http://earth.google.com/kml/2.1\"> <Document> <name>ais.kml</name> <Style id=\"sn_icon25_copy7\"> <IconStyle> <color>ff00ff00</color> <scale>0.9</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <StyleMap id=\"msn_icon25_copy6\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25_copy6</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17_copy6</styleUrl> </Pair> </StyleMap> <Style id=\"sh_icon17\"> <IconStyle> <scale>0.354545</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <LabelStyle> <color>ff0000ff</color> <scale>0.6</scale> </LabelStyle> </Style> <StyleMap id=\"msn_icon25_copy1\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25_copy1</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17_copy1</styleUrl> </Pair> </StyleMap> <Style id=\"sn_icon25_copy6\"> <IconStyle> <color>ff00ffff</color> <scale>0.9</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <StyleMap id=\"msn_icon25\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17</styleUrl> </Pair> </StyleMap> <Style id=\"sh_icon17_copy7\"> <IconStyle> <color>ff00ff00</color> <scale>0.590908</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <Style id=\"sh_icon17_copy1\"> <IconStyle> <color>ff0000ff</color> <scale>0.590908</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <Style id=\"sn_icon25\"> <IconStyle> <scale>0.3</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <LabelStyle> <color>ff0000ff</color> <scale>0.6</scale> </LabelStyle> </Style> <Style id=\"sn_icon25_copy1\"> <IconStyle> <color>ff0000ff</color> <scale>0.9</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <Style id=\"sh_icon17_copy6\"> <IconStyle> <color>ff00ffff</color> <scale>0.590908</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <StyleMap id=\"msn_icon25_copy7\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25_copy7</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17_copy7</styleUrl> </Pair> </StyleMap>";
static char *bottomkml = "</Document></kml>";
MYSQL_RES *query(MYSQL *conn, const char *sqlquery)
{
  if (mysql_query(conn,sqlquery))
    {
      fprintf(stderr, "%s\n",mysql_error(conn));
      exit(0);
    }
  return mysql_use_result(conn);
}

void lagkml()
{
  kmlfil.str("");
  kmlfil << headerkml;
  time_t nu;
  time(&nu);
  int gammelny;
  ostringstream oldpos;

  long mmsier[200];
  MYSQL conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  //char sqlquery[300] = "";
  ostringstream sqlquery;

  char *server = "localhost";
  char *user = "ruben";
  char *password = "elg";
  char *database = "diverse";
  int antallmmsi;
  int i,j;
  long mmsi;
  bool found = false;
  int posteller;
  float A,B,C,D;
  float height;
  double longitude, latitude;
  float coarse=0;
  float coarse2;
  int coarse3;
  double jordradius = 6372797;
  int status;
  double radiusvedlat, lonpermeter, latpermeter, buff, buff2, buff3, buff4;

  if(!mysql_init(&conn))
    {
      fprintf(stderr,"Feil \n");
      exit(0);
    }
  if (!mysql_real_connect(&conn,server,user,password, database, 0, NULL,0))
    {
      fprintf(stderr, "%s\n", mysql_error(&conn));
      exit(0);
    }
  sqlquery << "select distinct mmsi from ais_position where time > ";
  sqlquery << (int)nu-120;
  sqlquery << " order by mmsi";
  res = query(&conn,sqlquery.str().c_str());
  i = 0;

  while ((row = mysql_fetch_row(res)) != NULL)
    {
      mmsier[i++] = atoi(row[0]);
    }
  gammelny = i;
  mysql_free_result(res);
  sqlquery.str("");
  sqlquery << "select distinct mmsi from ais_position where time > ";
  sqlquery << (int)nu-600;
  sqlquery << " and time <= ";
  sqlquery << (int)nu-120;
  sqlquery << " order by mmsi";
  //printf("%s\n",sqlquery.str().c_str());
  res = query(&conn,sqlquery.str().c_str());
  while ((row = mysql_fetch_row(res)) != NULL)
    {
      //        printf("%s\n", row[0]);
      mmsi = atoi(row[0]);
      found = false;
      for(j=0;j<gammelny;j++)
        {
          if (mmsi == mmsier[j]) {found = true; break;}
        }
      if (!found)
        mmsier[i++] = atoi(row[0]);
    }
  mysql_free_result(res);
  antallmmsi = i;

  for(i=0;i<antallmmsi;i++)
    {

      //        printf("%d:",mmsier[i]);
      oldpos.str("");
      posteller = 0;
      sqlquery.str("");
      sqlquery << "select distinct name,A,B,C,D,height from ais_vesseldata where A != 0 and mmsi = ";
      sqlquery << mmsier[i];
      sqlquery << " order by time desc limit 1";
      res = query(&conn, sqlquery.str().c_str());

      if ((row = mysql_fetch_row(res)) != NULL)
        {
          //            printf(" - %s ",row[0]);
          kmlfil << "<Folder><name>" << row[0] << "</name><Placemark><name>";
          kmlfil << row[0];
          //                    if (strcmp(row[1],"NULL") != 0){
          //                            printf("%s\n",row[1]);
          A = atof(row[1]);
          B = atof(row[2]);
          C = atof(row[3]);
          D = atof(row[4]);
          height = (float)atof(row[5]);
        }
      else {kmlfil << "<Folder><name>" << mmsier[i] << "</name><Placemark><name>" << mmsier[i];
        A = 10;
        B = 10;
        C = 10;
        D = 10;
        height = 5;
      }
      mysql_free_result(res);

      kmlfil << "</name><description>";
      sqlquery.str("");
      sqlquery << "select distinct destination from ais_vesseldata where mmsi = ";
      sqlquery << mmsier[i];
      sqlquery << " and time > ";
      sqlquery << ((int)nu -3600);
      kmlfil << "MMSI: " << mmsier[i] << "\n ";
      res = query(&conn, sqlquery.str().c_str());

      if ((row = mysql_fetch_row(res)) != NULL)
        {
          //            printf(" - Destination: %s ",row[0]);
          kmlfil << "Destination: " << row[0];
        }
      else  kmlfil << " ";
      mysql_free_result(res);
      sqlquery.str("");
      sqlquery << "select time,coarse,heading,speed,longitude,latitude from ais_position where longitude != 0 and mmsi = ";
      sqlquery << mmsier[i];
      sqlquery << " order by id DESC limit 1";

      res = query(&conn,sqlquery.str().c_str());

      //        printf("\n");
      if ((row = mysql_fetch_row(res)) != NULL)
        {
          kmlfil << "\nSpeed: " << row[3] << "\nCoarse: " << row[1] << "\nHeading: " << row[2] <<  "\nLast update: " << row[0];
          longitude = atof(row[4]);
          latitude = atof(row[5]);
          coarse2 = atof(row[1]);
          status = 0;//atoi(row[6]);
          coarse3 = atoi(row[2]);
          if(coarse3 == 511) coarse = coarse2;
          else coarse = (float)coarse3;
        }
      else kmlfil << "\n";

      mysql_free_result(res);

      radiusvedlat = jordradius * cos(latitude*M_PI/180);
      lonpermeter = 360/(radiusvedlat*2*M_PI);
      latpermeter = 360/(jordradius*2*M_PI);

      kmlfil << "</description><visibility>1</visibility><LookAt><longitude>" << row[4] << "</longitude><latitude>" << row[5] << "</latitude><altitude>0</altitude><range>500</range><tilt>0</tilt><heading>0</heading></LookAt>";

      if (status != 0)
        kmlfil << "<styleUrl>#msn_icon25_copy1</styleUrl>";
      else if (i >= gammelny)
        kmlfil << "<styleUrl>#msn_icon25_copy6</styleUrl>";
      else if (status == 0)
        kmlfil << "<styleUrl>#msn_icon25_copy7</styleUrl>";

      kmlfil << "<Point><coordinates>" << row[4] << "," << row[5] << "</coordinates></Point></Placemark>";
      kmlfil << "<Placemark><name>modell</name><styleUrl>#shipmodell</styleUrl><Polygon><extrude>1</extrude><altitudeMode>relativeToGround</altitudeMode><outerBoundaryIs><LinearRing><coordinates>";

      buff = sqrt(B*B+D*D);
      buff2 = atan(B/D)*180/M_PI;
      buff2 += coarse + 90;
      buff3 = sin(buff2*M_PI/180)*buff*lonpermeter;
      buff4 = cos(buff2*M_PI/180)*buff*latpermeter;
      kmlfil << setprecision(9) << longitude+buff3 << "," << latitude+buff4 <<"," <<  height << " ";
      buff = sqrt(C*C+B*B);
      buff2 = atan(C/B)*180/M_PI;
      if (buff2 < 0) buff2 += 360;
      buff2 += coarse + 180;
      buff3 = sin(buff2*M_PI/180)*buff*lonpermeter;
      buff4 = cos(buff2*M_PI/180)*buff*latpermeter;
      kmlfil << setprecision(9) << longitude+buff3 << "," << latitude+buff4 <<"," <<  height << " ";
      buff = sqrt((A-(A/6))*(A-(A/6))+C*C);
      buff2 = atan((A-(A/6))/C)*180/M_PI;
      if (buff2 < 0) buff2 += 360;
      buff2 += coarse + 270;
      buff3 = sin(buff2*M_PI/180)*buff*lonpermeter;
      buff4 = cos(buff2*M_PI/180)*buff*latpermeter;
      kmlfil << setprecision(9) << longitude+buff3 << "," << latitude+buff4 <<"," <<  height << " ";
      buff = sqrt((A)*(A)+(D-(C+D)/2)*(D-(C+D)/2));;
      buff2 = atan((D-(C+D)/2)/A)*180/M_PI;
      if (buff2 < 0) buff2 += 360;
      buff2 += coarse + 0;
      buff3 = sin(buff2*M_PI/180)*buff*lonpermeter;
      buff4 = cos(buff2*M_PI/180)*buff*latpermeter;
      kmlfil << setprecision(9) << longitude+buff3 << "," << latitude+buff4 <<"," <<  height << " ";
      buff = sqrt((A-(A/6))*(A-(A/6))+D*D);
      buff2 = atan(D/(A-(A/6)))*180/M_PI;
      if (buff2 < 0) buff2 += 360;
      buff2 += coarse + 0 ;
      buff3 = sin(buff2*M_PI/180)*buff*lonpermeter;
      buff4 = cos(buff2*M_PI/180)*buff*latpermeter;
      kmlfil << setprecision(9) << longitude+buff3 << "," << latitude+buff4 <<"," <<  height << " ";
      buff = sqrt(B*B+D*D);
      buff2 = atan(B/D)*180/M_PI;
      buff2 += coarse + 90;
      buff3 = sin(buff2*M_PI/180)*buff*lonpermeter;
      buff4 = cos(buff2*M_PI/180)*buff*latpermeter;
      kmlfil << setprecision(9) << longitude+buff3 << "," << latitude+buff4 <<"," <<  height << " ";
      /*buff = sqrt(B*B+D*D);
        buff2 = atan2(D,-B)*180/M_PI;
        if (buff2 > 0) buff2 += 360;
        buff2 += coarse;
        buff3 = cos(buff2*M_PI/180)*buff*lonpermeter;
        buff4 = sin(buff2*M_PI/180)*buff*latpermeter;
        kmlfil << setprecision(9) << longitude+buff3 << "," << latitude+buff4 <<"," <<  height << " ";*/
      kmlfil << "</coordinates></LinearRing></outerBoundaryIs></Polygon></Placemark>";
      kmlfil << "<Placemark><name>Spor " << mmsier[i] << "</name><LineString><tessellate>1</tessellate><coordinates>";
      sqlquery.str("");
      sqlquery << "select heading,time,longitude,latitude, coarse,speed from ais_position where mmsi = ";
      sqlquery << mmsier[i];
      sqlquery << " and time > ";
      sqlquery << ((int)nu-(3600*5));
      sqlquery << " order by time desc";

      res = query(&conn,sqlquery.str().c_str());

      //printf("\n");
      while ((row = mysql_fetch_row(res)) != NULL)
        {
          //                    printf("  %s %s %s %s %10s %10s\n",row[0],row[1],row[2],row[3],row[4],row[5]);
          kmlfil << row[2] << "," << row[3] << ",0 ";
          if ((posteller % 10) ==  9)
            {
              oldpos << "<Placemark><name>" << row[1] << "</name><styleUrl>#msn_icon25</styleUrl><Point><coordinates>" << row[2] << "," << row[3] << ",0</coordinates></Point></Placemark>";
            }
          posteller++;

        }
      mysql_free_result(res);

      kmlfil << " </coordinates></LineString></Placemark><Folder><name>oldpositions</name>" << oldpos.str() << "</Folder></Folder>";

    }

  mysql_close(&conn);
  kmlfil << bottomkml;
}

void server()
{
  int sockfd, clilen, klientsock;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(12350);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
      serv_addr.sin_port = htons(12351);
      if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
          fprintf(stderr, "Feil\n");
          exit(-1);
        }
    }
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  while(1)
    {
      klientsock = accept(sockfd, (struct sockaddr*)&cli_addr, (socklen_t *)&clilen);
      if(klientsock < 0)
        {
          fprintf(stderr, "Error on socket accept\n");
          exit(-1);
        }
      char *buffer = (char *)malloc(1000*sizeof(char));
      read(klientsock,buffer,1000);
      free(buffer);
      lagkml();
      string kml = kmlfil.str();
      write(klientsock,kml.c_str(),kml.size());
      shutdown(klientsock,2);
      close(klientsock);
    }

}

int main()
{
  server();
  cout << kmlfil.str() << "\n";
}
