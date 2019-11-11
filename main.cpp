#include <QtCore/QCoreApplication>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <QtXml>
#include <QDomDocument>
#include <QFile>
#include <cmath>

using namespace std;

struct route
{
    string airline,airlineid;
};
struct direct
{
    string theori,thedes;
};

struct airport
{
    QString name,city,country,iata,icao,lat,lon;
};

void readFromXml(QDomDocument &doc, const QString &filename);
void loadAirportMap(map<string,airport> &airportMap, QDomDocument &airports, string &ori, string &des);
void loadroute(map<string,airport> &airportMap,  vector<pair<direct,route> > &airroute,QDomDocument &airports, QDomDocument &routes);
void getinput(string &ori, string &des);
double thedistance(map<string,airport> &airportMap);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDomDocument airports, routes, airlines;
    map<string,airport> airportMap;
    vector<pair<direct,route> > airroute;
    readFromXml(airports,"airports.xml");
    readFromXml(routes,"routes.xml");
    readFromXml(airlines,"airlines.xml");
    system("cls");
    string ori, des;
    getinput(ori,des);
    loadAirportMap(airportMap, airports,ori,des);
    loadroute(airportMap,airroute,airports,routes);
    return 0;
}

void getinput(string& ori, string& des)
{
    cout <<"please input the origin airport"<<endl;
    cin >>ori;
    transform(ori.begin(), ori.end(), ori.begin(), ::toupper);
    cout <<"please input where you want to go"<<endl;
    cin >>des;
    transform(des.begin(), des.end(), des.begin(), ::toupper);
}
void loadAirportMap(map<string,airport> &airportMap, QDomDocument &airports, string &ori, string &des)
{

    QDomNodeList elements = airports.documentElement().elementsByTagName( "airport" );
    airport port;
    int count=0;
    for(size_t i = 0; i < elements.size(); ++i)
    {
        QDomNode curr = elements.item(i);
        QString id = curr.firstChildElement("id").text();
        port.iata = curr.firstChildElement("iata").text();
        string check = port.iata.toStdString();
        if(ori.compare(check) == 0)
        {
            count++;
            port.name = curr.firstChildElement("name").text();
            port.iata = curr.firstChildElement("iata").text();
            port.icao = curr.firstChildElement("icao").text();
            port.country = curr.firstChildElement("country").text();
            port.city = curr.firstChildElement("city").text();
            port.lon = curr.firstChildElement("longitude").text();
            port.lat = curr.firstChildElement("latitude").text();
            airportMap.insert(pair<string, airport>("ori",port));
        }
        if(des.compare(check) == 0)
        {
            count++;
            port.name = curr.firstChildElement("name").text();
            port.iata = curr.firstChildElement("iata").text();
            port.icao = curr.firstChildElement("icao").text();
            port.country = curr.firstChildElement("country").text();
            port.city = curr.firstChildElement("city").text();
            port.lon = curr.firstChildElement("longitude").text();
            port.lat = curr.firstChildElement("latitude").text();
            airportMap.insert(pair<string, airport>("des",port));
        }
        if(count == 2) break;
   }
   if(count !=2)
   {
       cout << "the name you input is incorrect"<<endl;
       exit(0);
   }

}
void loadroute(map<string,airport> &airportMap, vector<pair<direct,route> > &airroute, QDomDocument &airports, QDomDocument &routes)
{
    QDomNodeList elements = routes.documentElement().elementsByTagName("route" );
    string des = airportMap["des"].iata.toStdString();
    string ori = airportMap["ori"].iata.toStdString();
    string loc1, loc2, temp1,temp2;
    direct direction;
    route theroute;
    int count=0;
    for(size_t i = 0; i < elements.size(); ++i)
    {
        QDomNode curr = elements.item(i);
        temp1 = curr.firstChildElement("destinationAirport").text().toStdString();
        if(temp1.compare(des) == 0)
        {
            loc1 = curr.firstChildElement("sourceAirport").text().toStdString();
            loc2 = temp1;
            theroute.airline =  curr.firstChildElement("airline").text().toStdString();
            theroute.airlineid =  curr.firstChildElement("airlineID").text().toStdString();
            direction.theori = loc1;
            direction.thedes = loc2;
            airroute.push_back(pair<direct,route>(direction,theroute));
            if(loc1 == ori) break;
            for(size_t j = 0; j < elements.size(); ++j)
            {
                 QDomNode next = elements.item(j);
                 temp2 = next.firstChildElement("destinationAirport").text().toStdString();
                 if(temp2.compare(loc1) == 0)
                 {
                    loc2 = loc1;
                    loc1 = next.firstChildElement("sourceAirport").text().toStdString();
                    if(loc1 == ori)
                    {
                        direction.theori = loc1;
                        direction.thedes = loc2;
                        theroute.airline =  next.firstChildElement("airline").text().toStdString();
                        theroute.airlineid =  next.firstChildElement("airlineID").text().toStdString();
                        airroute.push_back(pair<direct,route>(direction,theroute));
                        break;
                    }
                    else loc1 = loc2;
                 }
                 if((j+1) == elements.size()) airroute.pop_back();
             }
        }
   }
   cout<<"from "<<ori <<" to "<<des<<":"<<endl;

   if(airroute.size() ==0) cout<<"did not find any airline"<<endl;
   else
   {
       vector<pair<direct,route> >::reverse_iterator it = airroute.rbegin();
       while (it != airroute.rend())
       {
           map<string,airport> thetemp;
           loadAirportMap(thetemp,airports,it->first.theori,it->first.thedes);
           if(count==0) cout<<endl<<"The shortest route is:"<<endl;
           cout<<"leave "<<it->first.theori<<"("<<thetemp["ori"].name.toStdString()<<")"<<" and fly ";
           cout <<thedistance(thetemp);
           cout <<" km arrive "<<it->first.thedes<<"("<<thetemp["des"].name.toStdString()<<")"<<endl;
           if(it->first.thedes == des)
           {
               count++;
               cout <<"-----------"<<endl;
               if(count==1||count==2) cout<<endl<<"The rest routes are:"<<endl;
           }
           count++;
           ++it;
       }

   }
}

double thedistance(map<string,airport> &airportMap)
{
    double dist,lat1, lat2, lon1, lon2, lat, lon, a, c, d_to_r = 0.0174533;
    lat1 = airportMap["ori"].lat.toDouble()*d_to_r;
    lat2 = airportMap["des"].lat.toDouble()*d_to_r;
    lat  = abs(lat2 - lat1);
    lon1 = airportMap["ori"].lon.toDouble()*d_to_r;
    lon2 = airportMap["des"].lon.toDouble()*d_to_r;
    lon  = abs(lon2 - lon1);
    a = pow(sin(lat/2),2.0)+cos(lat1)*cos(lat2)*pow(sin(lon/2),2.0);
    c = 2*atan2(sqrt(a),sqrt(1.0-a));
    dist = 6373 * c;

    return dist;
}

void readFromXml(QDomDocument &doc, const QString &filename)
{
    QFile file(filename);
    cout<<"Reading "<<filename.toStdString()<<endl;
    if (!file.open(QIODevice::ReadOnly) )
    {
        qDebug()<<"Cannot open "<<filename<<"!";
        return;
    }
    if (!doc.setContent(&file))
    {
        qDebug()<<"Cannot parse "<<filename<<"!";
           return;
     }
    cout<<"Finished reading "<<filename.toStdString()<<endl;
}
