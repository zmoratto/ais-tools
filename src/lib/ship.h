#ifndef INC_SHIP_HH
#define INC_SHIP_HH

#include <iostream>
#include <vector>
#include <time.h>

typedef struct {
	double longitude;
	double latitude;
	unsigned short heading;
	unsigned short speed;
	time_t tid;

} posdata;

class Ship
{
	private:
		unsigned long mmsi;
//		double longitude;
//		double latitude;
		std::vector<posdata> gamle;
//		unsigned short heading;
		std::string name;
		std::string destination;
		bool old, visible;
		int lastbra;
		bool hasname;

	public:
		Ship();
		std::string get_kml();
		unsigned long get_mmsi();
		void set_name(std::string name);
		void set_destination(std::string destination);
		bool check_time();
		void update_pos(double latitude, double longitude, unsigned short heading, unsigned short speed);

		void set_mmsi(unsigned long mmsi);
		bool has_name();
		void update_data(std::string name,std::string destination);

};


#endif
