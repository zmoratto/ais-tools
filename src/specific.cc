#include <cstdio>
#include <string.h>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <math.h>
#include <netinet/in.h>

#include <iomanip>
#include <mysql/mysql.h>
using namespace std;

ostringstream kmlfil;
string destination;
double A,B,C,D,height;

static char *headerkml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <kml xmlns=\"http://earth.google.com/kml/2.1\"> <Document> <name>ais.kml</name> <Style id=\"sn_icon25_copy7\"> <IconStyle> <color>ff00ff00</color> <scale>0.9</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <StyleMap id=\"msn_icon25_copy6\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25_copy6</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17_copy6</styleUrl> </Pair> </StyleMap> <Style id=\"sh_icon17\"> <IconStyle> <scale>0.354545</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <LabelStyle> <color>ff0000ff</color> <scale>0.6</scale> </LabelStyle> </Style> <StyleMap id=\"msn_icon25_copy1\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25_copy1</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17_copy1</styleUrl> </Pair> </StyleMap> <Style id=\"sn_icon25_copy6\"> <IconStyle> <color>ff00ffff</color> <scale>0.9</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <StyleMap id=\"msn_icon25\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17</styleUrl> </Pair> </StyleMap> <Style id=\"sh_icon17_copy7\"> <IconStyle> <color>ff00ff00</color> <scale>0.590908</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <Style id=\"sh_icon17_copy1\"> <IconStyle> <color>ff0000ff</color> <scale>0.590908</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <Style id=\"sn_icon25\"> <IconStyle> <scale>0.3</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <LabelStyle> <color>ff0000ff</color> <scale>0.6</scale> </LabelStyle> </Style> <Style id=\"sn_icon25_copy1\"> <IconStyle> <color>ff0000ff</color> <scale>0.9</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon25.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <Style id=\"sh_icon17_copy6\"> <IconStyle> <color>ff00ffff</color> <scale>0.590908</scale> <Icon> <href>http://maps.google.com/mapfiles/kml/pal4/icon17.png</href> </Icon> </IconStyle> <ListStyle> </ListStyle> </Style> <StyleMap id=\"msn_icon25_copy7\"> <Pair> <key>normal</key> <styleUrl>#sn_icon25_copy7</styleUrl> </Pair> <Pair> <key>highlight</key> <styleUrl>#sh_icon17_copy7</styleUrl> </Pair> </StyleMap>";
static char *bottomkml = "</Document></kml>";
char *server = "localhost";
char *user = "aisblock";
char *password = "elgelg";
char *database = "diverse";
MYSQL_RES *query(MYSQL *conn, const char *sqlquery)
{
  if (mysql_query(conn,sqlquery))
    {
      fprintf(stderr, "%s\n",mysql_error(conn));
      exit(0);
    }
  return mysql_store_result(conn);
}

string finnavn(MYSQL *conn,unsigned long mmsi)
{
  ostringstream queryen;
  ostringstream mmsien;
  queryen << "select distinct name from ais where type = 5 and mmsi = ";
  queryen << mmsi;
  queryen << " order by epoch desc limit 1";
  mmsien.str("");
  MYSQL_ROW row;
  MYSQL_RES *res = query(conn, queryen.str().c_str());
  if ((row = mysql_fetch_row(res)) != NULL)
    {
      mmsien << row[0];
    }
  else {
    mmsien << mmsi;
  }

  mysql_free_result(res);
  queryen.str("");
  queryen << "select distinct name,A,B,C,D,height,destination from ais where A != 0 and type = 5 and mmsi = ";
  queryen << mmsi;
  queryen << " order by epoch desc limit 1";
  res = query(conn, queryen.str().c_str());
  if ((row = mysql_fetch_row(res)) != NULL)
    {
      A = atof(row[1]);
      B = atof(row[2]);
      C = atof(row[3]);
      D = atof(row[4]);
      height = atof(row[5]);
      destination = row[6];
    }
  else {
    A =10;
    B =10;
    C = 10;
    D = 10;
    height = 5;
    destination = "";
  }

  mysql_free_result(res);

  return mmsien.str();
}

void lagkml(const char *queryen)
{
  kmlfil.str("");
  kmlfil << headerkml;
  time_t nu;
  int gammelny;
  int lastmmsi = 0;
  ostringstream oldpos;
  double radiusvedlat, lonpermeter, latpermeter, buff, buff2, buff3, buff4;
  double longitude,latitude,coarse;
  double speed;
  double jordradius = 6372797;
  time(&nu);
  long mmsier[200];
  MYSQL conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  ostringstream sqlquery;

  int antallmmsi;
  int i,j;
  long mmsi;
  bool found = false;
  int posteller;

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
  sqlquery << "select distinct mmsi,date,time,longitude,latitude,coarse,speed from ais where type = 1 ";
  sqlquery << queryen;
  sqlquery << " order by mmsi,date desc,time desc";
  res = query(&conn,sqlquery.str().c_str());
  i = 0;
  while ((row = mysql_fetch_row(res)) != NULL)
    {
      //        printf("%s %s %s %s %s\n", row[1],row[2],row[0],row[3],row[4]);
      mmsi = atoi(row[0]);
      if (lastmmsi != mmsi){
        if (lastmmsi != 0) kmlfil << "</coordinates></LineString></Placemark></Folder>";
        kmlfil << "<Folder><name>" << row[0] << "</name><Placemark><name>";
        kmlfil << finnavn(&conn,mmsi);
        kmlfil << "</name>";
        kmlfil << "<description>";
        kmlfil << "</description>";
        longitude = atof(row[3]);
        latitude = atof(row[4]);
        coarse =  atof(row[5]);
        speed = atof(row[6]);
        radiusvedlat = jordradius * cos(latitude*M_PI/180);
        lonpermeter = 360/(radiusvedlat*2*M_PI);
        latpermeter = 360/(jordradius*2*M_PI);
        kmlfil << "<description>";
        kmlfil << "MMSI: " << mmsi << "\n";
        kmlfil << "Speed: " << speed << " knots\n";
        kmlfil << "Coarse: " << coarse << " degrees\n";
        kmlfil << "Last update: " << row[1] << " " << row[2] <<  "\n";
        kmlfil << "Destination: " << destination << "\n";
        kmlfil << "</description>";
        kmlfil << "<visibility>1</visibility><LookAt><longitude>" << row[3] << "</longitude><latitude>" << row[4] << "</latitude><altitude>0</altitude><range>500</range><tilt>0</tilt><heading>0</heading></LookAt>";
        kmlfil << "<styleUrl>#msn_icon25_copy7</styleUrl>";
        kmlfil << "<Point><coordinates>" << row[3] << "," << row[4] << "</coordinates></Point></Placemark>";
        kmlfil << "<Placemark><name>" << row[1] << " " << row[2] << "</name><styleUrl>#msn_icon25</styleUrl><Point><coordinates>" << row[3] << "," << row[4] << ",0</coordinates></Point></Placemark>";
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

        kmlfil << "<Placemark><name>Spor " << mmsi << "</name><LineString><tessellate>1</tessellate><coordinates>";
      }
      kmlfil << row[3] << "," << row[4] << ",0 ";
      if ((i % 15) == 14)
        {
          oldpos << "<Placemark><name>"<< row[1] << " " << row[2] << "</name><visibility>0</visibility><styleUrl>#msn_icon25</styleUrl><Point><coordinates>" << row[3] << "," << row[4] << ",0</coordinates></Point></Placemark>";
        }
      lastmmsi = mmsi;
      i++;
    }
  kmlfil << "</coordinates></LineString></Placemark></Folder>";
  mysql_free_result(res);
  mysql_close(&conn);
  kmlfil << "<Folder><name>Oldpositions</name><visibility>0</visibility>"<<oldpos.str()<<"</Folder>";
  kmlfil << bottomkml;
}


int main(int argc, char **argv)
{
  if(argc != 2){
    printf("Feil\n");
    exit(-1);
  }
  lagkml(argv[1]);
  FILE *ut = fopen("spesifikk.kml","w");
  //    cout << kmlfil.str();
  fprintf(ut,kmlfil.str().c_str());
  fclose(ut);
}
