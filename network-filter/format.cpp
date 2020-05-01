#include "pch.h"
#include "format.h"

STRING net_speed_format(DOUBLE bytes)
{
 STRING q = "";
 if (bytes >= 1024) {
	 q = "K";
	 bytes /= 1024;
	 if (bytes >= 1024) {
		 q = "M";
		 bytes /= 1024;
		 if (bytes >= 1024) {
			 q = "G";
			 bytes /= 1024;
		 }
	 }
 }
 STRING result = std::to_string((int)round(bytes * 100));
 STRING res;
 if (result.size() < 3) res = "0.00";
 else res = result.insert(result.size() - 2, 1, '.');
 return res + " " + q + "B/s";
}