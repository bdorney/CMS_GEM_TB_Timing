
#ifndef ____multiDetTimeResUtility__
#define ____multiDetTimeResUtility__

//C++ Includes
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>

namespace{

bool convert2bool(std::string str, bool &bExitSuccess){
    //convert to upper case
    transform(str.begin(), str.end(), str.begin(), toupper);
    
    //Empty String?
    if ( str.empty() ) {
        bExitSuccess = false;
        return false;
    }
    
    //Input recognized?
    if ( !(str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0
           || str.compare("F")==0 || str.compare("FALSE")==0 || str.compare("0")==0) ) {
        bExitSuccess = false;
        return false;
    }
    
    bExitSuccess = true;
    
    return (str.compare("T")==0 || str.compare("TRUE")==0 || str.compare("1")==0);
} //End convert2bool()

struct cmp_chan{
    bool operator()(const std::string &str1, const std::string &str2) const{
        std::string alphabet = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz_";
        
        return std::stoi(str1.substr(str1.find_first_not_of(alphabet), str1.length() - str1.find_first_not_of(alphabet) ) ) > std::stoi(str2.substr(str2.find_first_not_of(alphabet), str2.length() - str2.find_first_not_of(alphabet) ) );
    }
};

/*struct cmp_mappedVal{
    bool operator()(std::map<std::string,int> map1, std::map<std::string,int> map2) const{
        return map1.second < map2.second;
    }
};*/

typedef std::pair<std::string, int> PairType_Str_Int;
struct CompareSecond_Min
{
    bool operator()(const PairType_Str_Int& left, const PairType_Str_Int& right) const
    {
        return left.second < right.second;
    }
};
struct CompareSecond_Max
{
    bool operator()(const PairType_Str_Int& left, const PairType_Str_Int& right) const
    {
        //return left.second > right.second;
	return left.second < right.second;
    }
};

int getMinForChannelOR(std::map<std::string, int> inputMap){
    //Variable Declaration
    std::map<std::string, int>::iterator iterMap = inputMap.begin();
    std::map<std::string, int>::iterator iterMapEnd = inputMap.end();

    while(iterMap != iterMapEnd){
	if( 0 == (*iterMap).second){
		std::map<std::string, int>::iterator iterTemp = iterMap;
		++iterMap;
		inputMap.erase(iterTemp);

		iterMapEnd = inputMap.end(); //??

		if(iterMap == iterMapEnd) break;
	}
	else{
		++iterMap;

		if(iterMap == iterMapEnd) break;
	}
    }
    
    std::pair<std::string, int> min = *min_element(inputMap.begin(), inputMap.end(), CompareSecond_Min());
    
    return min.second;
}

int getMaxForChannelAND(std::map<std::string, int> inputMap){
    //Variable Declaration
    int iRetVal;
    
    //Require
    if ( getMinForChannelOR(inputMap) > 0 ) {
        std::pair<std::string, int> max = *max_element(inputMap.begin(), inputMap.end(), CompareSecond_Max() );
        
        //iRetVal = getMaxForChannelAND(inputMap);
        iRetVal = max.second;
    }
    else{
        //One or more channels off
        iRetVal = -1;
    }
    
    return iRetVal;
} //End getMaxForChannel

} //End namespace

#endif
