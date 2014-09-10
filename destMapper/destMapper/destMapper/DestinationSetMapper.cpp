// DestinationSetMapper.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <string>
#include <forward_list>
#include <list>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <ostream>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#define MIN(a, b) (a>b?b:a)
#define MAX(a, b) (a>b?a:b)

using namespace std;

namespace mp = boost::multiprecision;
typedef mp::cpp_dec_float_50 hp_float;



string padRightTo(std::string &str, const size_t num, const char paddingChar = ' ')
{
	if (num > str.size())
		str.insert(str.size(), num - str.size(), paddingChar);
	return str;
}

inline string convertToString(double const& d)
{
	std::ostringstream strs;
	strs << d;
	std::string str = strs.str();
	return str;
}

inline string convertToString(hp_float const& d)
{
	std::ostringstream strs;
	strs << std::setprecision(std::numeric_limits<mp::cpp_dec_float_100>::max_digits10) << d;
	std::string str = strs.str();
	return str;
}

class Segment {
public:
	friend std::ostream& operator<< (std::ostream& o, Segment& s);
    const string get_range();
	Segment(string, string);
	Segment(string, hp_float, hp_float);
	string name;
	hp_float start;
	hp_float end;
};

const string Segment::get_range()
{
    string s_start, s_end;
	s_start = convertToString(start).substr(2, string::npos);
	s_end = boost::lexical_cast<string>(boost::lexical_cast<int64_t>(convertToString(end).substr(2, string::npos)) - 1);
    s_start = padRightTo(s_start, s_end.size(), '0');
    s_end = padRightTo(s_end, s_start.size(), '9');
    if (s_start == s_end)
        return s_start;
    return s_start + "-" + s_end;
    
}

std::ostream& operator<< (std::ostream& o, Segment& s)
{
	
	return o << s.name << ": " << s.get_range();

}

Segment::Segment(string n, string a)
{
	using boost::lexical_cast;

	name = n;
	try {
		start = lexical_cast<hp_float>("0." + a);
		end = lexical_cast<hp_float>("0." + lexical_cast<string>(lexical_cast<int64_t>(a)+1));
	}
	catch (exception& e)
	{
		cerr << a << " ";
		cerr << e.what() << endl;
	}
}

Segment::Segment(string n, hp_float s, hp_float e)
{
	name = n;
	start = s;
	end = e;
}

class Destination
{
public:
    string name;
    vector<Segment> areas;
    string get_compact_area();
};



bool sortFunction(Segment a, Segment b)
{
	return a.start < b.start;
}

string Destination::get_compact_area()
{
    stringstream sres;
    sort(areas.begin(), areas.end(), sortFunction);
    vector<Segment> compact;
    Segment *temp = nullptr;
    for (vector<Segment>::iterator it = areas.begin(); it != areas.end(); it++)
    {
        if (temp == nullptr || (temp != nullptr && temp->end != it->start))
        {
            temp = new Segment(it->name, it->start, it->end);
            compact.push_back(*temp);
        }
        else
        {
            temp->end = it->end;
        }
        
    }
    for (vector<Segment>::iterator it = compact.begin(); it != compact.end(); it++)
    {
        sres << it->get_range() << ", ";
    }
    return sres.str();
}


vector<Segment> ADests, BDests;

vector<Segment> get_explicit_set(vector<Segment> origSet)
{
	sort(origSet.begin(), origSet.end(), sortFunction);
	list<Segment> origList;
	for (vector<Segment>::iterator it = origSet.begin(); it != origSet.end(); ++it)
	{
		origList.push_back(*it);
	}
	vector<Segment> explSet;
	list<Segment>::iterator seg = origList.begin();
	list<Segment>::iterator nextSeg;
	while (seg != origList.end())
	{
		nextSeg = seg;
		++nextSeg;

		while (seg != origList.end() && nextSeg != origList.end() && seg->end <= nextSeg->start)
		{
			explSet.push_back(*seg);
			++seg;
			nextSeg = seg;
			++nextSeg;
		}
		if (nextSeg == origList.end())
		{
			explSet.push_back(*seg);
			return explSet;
		}
		else
		{
			if (seg->start < nextSeg->start)
			{
				explSet.push_back(Segment(seg->name, seg->start, nextSeg->start));
			}
			if (nextSeg->end < seg->end)
			{
				Segment newSegment = Segment(seg->name, nextSeg->end, seg->end);
				list<Segment>::iterator its = nextSeg;
				while (its != origList.end() && its->end <= newSegment.start)
					++its;
				if (its == origList.end())
				{
					origList.push_back(newSegment);
				}
				else
				{
					origList.insert(its, newSegment);
				}
				
			}
			++seg;
		}
	}
	return explSet;

}

vector<Segment> map_destination_sets(vector<Segment> dest_a, vector<Segment> dest_b)
{
	vector<Segment> e_a = get_explicit_set(dest_a);
	vector<Segment> e_b = get_explicit_set(dest_b);
	vector<Segment>::iterator it_a, it_b;
	it_a = e_a.begin();
	it_b = e_b.begin();
	vector<Segment> mapped_result;

	while (it_a != e_a.end() && it_b != e_b.end())
	{
		if (it_b->end <= it_a->start)
		{
			it_b++;
		}
		else
		{
            if (it_b->start < it_a->end)
                mapped_result.push_back(Segment(it_a->name + "->" + it_b->name, MAX(it_a->start, it_b->start), MIN(it_a->end, it_b->end)));

			if (it_b->end < it_a->end)
				it_b++;
			else
				it_a++;
		}
	}
	return mapped_result;
}

void print_dests(vector<Segment> dests, ostream *out)
{
    *out << dests.size() << endl;
    for (vector<Segment>::iterator it = dests.begin(); it != dests.end(); ++it)
    {
        *out << *it << endl;
    }
}

void readFile(istream *f)
{
    int n;
    string name;
    string area;
    
	*f >> n;
	for (int i = 0; i < n; i++)
	{
		*f >> name;
		*f >> area;
		Segment s = Segment(name, area);
		ADests.push_back(s);
	}
	
	*f >> n;
	for (int i = 0; i < n; i++)
	{
		
		*f >> name;
		*f >> area;
		Segment s = Segment(name, area);
		BDests.push_back(s);
	}
}

map<string, Destination> create_destinations (vector<Segment> ds)
{
    map<string, Destination> res;
    map<string, Destination>::iterator el;
    for (vector<Segment>::iterator it = ds.begin(); it != ds.end(); it++)
    {
        
        if ((el = res.find(it->name)) != res.end())
        {
            el->second.areas.push_back(*it);
        }
        else
        {
            Destination d;
            d.name = it->name;
            d.areas.push_back(*it);
            res[d.name] =  d;
        }
    }
    
    return res;
}

int main(int argc, const char* argv[])
{

    if (argc > 1) {
        ifstream fin;
        fin.open(argv[1], ios::in);
        if (!fin)
            return 1;
        readFile(&fin);
    }
    else
    {
        readFile(&cin);
    }
    
	try {
	
		//print_dests(ADests);
        //print_dests(BDests);
		
		std::cout << "Mapped" << endl;
		vector<Segment> mapped = map_destination_sets(ADests, BDests);
        
        map<string, Destination> dd = create_destinations(mapped);
		ofstream outf;
        outf.open("out.txt", ios::out);
        //print_dests(mapped, &outf);
        
        for (map<string, Destination>::iterator it=dd.begin(); it != dd.end(); it++)
        {
            outf << it->first << "-> (" << it->second.get_compact_area() << ")" << endl;
        }
        
        outf.close();
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
	
}

