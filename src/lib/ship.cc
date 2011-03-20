#include <iostream>


#include <cstdio>
#include "ship.h"



Ship::Ship()
{
	visible = true;
	old = false;
	lastbra = 0;
	hasname = false;
	destination = "";
}

std::string Ship::get_kml()
{
	char tallbuffer[80];
	struct tm ts;

	std::string tmp;
	if (visible){
	tmp.append("<Placemark><name>");
	tmp.append(name);
	tmp.append("</name><description>Speed: ");
	sprintf(tallbuffer,"%.1f",(float)gamle[gamle.size()-1].speed/10);
	tmp.append(tallbuffer);
	tmp.append("knots\nLast update: ");
	ts = *localtime(&gamle[gamle.size()-1].tid);
	strftime(tallbuffer, sizeof(tallbuffer),"%a %Y-%m-%d %H:%M:%S %Z",&ts);
	tmp.append(tallbuffer);
	tmp.append("\nDestination: ");
	tmp.append(destination);
	tmp.append("\nMMSI: ");
	sprintf(tallbuffer,"%09d",mmsi);
	tmp.append(tallbuffer);
	tmp.append("</description><visibility>1</visibility><LookAt><longitude>");
	sprintf(tallbuffer,"%.10f",gamle[gamle.size()-1].longitude);
	tmp.append(tallbuffer);
	tmp.append("</longitude><latitude>");
	sprintf(tallbuffer,"%.10f",gamle[gamle.size()-1].latitude);
	tmp.append(tallbuffer);
	tmp.append("</latitude><altitude>0</altitude><range>9042.991916960778</range><tilt>70</tilt><heading>");
	sprintf(tallbuffer,"%.2f",((float)gamle[gamle.size()-1].heading)/10);
	tmp.append(tallbuffer);
	tmp.append(" </heading></LookAt>");
	if (!old)
		tmp.append("<styleUrl>#msn_ylw-circle</styleUrl><Point><coordinates>");
	else
		tmp.append("<styleUrl>#msn_red-diamond</styleUrl><Point><coordinates>");
	//msn_red-diamond
	sprintf(tallbuffer,"%.10f",gamle[gamle.size()-1].longitude);
	tmp.append(tallbuffer);
	tmp.append(",");
	sprintf(tallbuffer,"%.10f",gamle[gamle.size()-1].latitude);
	tmp.append(tallbuffer);
	tmp.append("</coordinates></Point></Placemark><Placemark><name>Spor ");
	tmp.append(name);
	tmp.append("</name><LineString><tessellate>1</tessellate><coordinates>");
	for(int i=lastbra;i<gamle.size();i++)
	{
	sprintf(tallbuffer,"%.10f",gamle[i].longitude);
	tmp.append(tallbuffer);
	tmp.append(",");
	sprintf(tallbuffer,"%.10f",gamle[i].latitude);
	tmp.append(tallbuffer);
	tmp.append(",0 ");
	}
	

	tmp.append(" </coordinates></LineString></Placemark>");
	}
	return tmp;
}
bool Ship::has_name()
{
	return hasname;
}

void Ship::update_data(std::string name, std::string destination)
{
	set_name(name);
	set_destination(destination);

	hasname = true;
	
}

unsigned long Ship::get_mmsi()
{
	return mmsi;

}
void Ship::set_mmsi(unsigned long mmsi)
{
	this->mmsi = mmsi;
}

void Ship::set_name(std::string name)
{

	this->name = name;

}

void Ship::set_destination(std::string destination)
{
	this->destination = destination;
}

bool Ship::check_time()
{
	time_t nu;	
	time(&nu);
	if ((int)nu > ((int)gamle[gamle.size()-1].tid + 600))
	{
		visible = false;
		lastbra = gamle.size();
	}
	else if ((int)nu > ((int)gamle[gamle.size()-1].tid + 120))
	{
		old = true;

	}



}

void Ship::update_pos(double latitude, double longitude, unsigned short heading, unsigned short speed)
{
	posdata posisjon;
	if (gamle.size() > 0){
	if ((latitude > (gamle[gamle.size()-1].latitude + 0.000001)) || (longitude > (gamle[gamle.size()-1].longitude + 0.000001)) || (latitude < (gamle[gamle.size()-1].latitude - 0.000001)) || (longitude < (gamle[gamle.size()-1].longitude - 0.000001))){
	posisjon.latitude = latitude;
	posisjon.longitude = longitude;
	posisjon.heading = heading;
	posisjon.speed = speed;
	 time(&posisjon.tid);
	gamle.push_back(posisjon);
	}
	else  {
		time(&gamle[gamle.size()-1].tid);
	}
	}
	else{
	posisjon.latitude = latitude;
	posisjon.longitude = longitude;
	posisjon.heading = heading;
	posisjon.speed = speed;
	 time(&posisjon.tid);
	gamle.push_back(posisjon);
	}
	visible = true;
	old = false;
}

