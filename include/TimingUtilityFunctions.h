
#ifndef ____TimingUtility__
#define ____TimingUtility__

//C++ Includes
#include <algorithm>
#include <iostream>
#include <map>
#include <stdio.h>
#include <sstream>
#include <string>

//My Includes
#include "TimingUtilityOperators.h"

//ROOT Includes

/*
 *
 *
 *
 *
 */

namespace Timing {
    bool convert2bool(std::string str, bool &bExitSuccess){
        //convert to upper case
        std::transform(str.begin(), str.end(), str.begin(), toupper);
        
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
    
    int getMinForChannelOR(std::map<std::string, int> inputMap){
        //Variable Declaration
        std::map<std::string, int>::iterator iterMap = inputMap.begin();
        std::map<std::string, int>::iterator iterMapEnd = inputMap.end();

        while( iterMap != inputMap.end() ){
        if( 0 == (*iterMap).second){
            //C++11 is magic
            iterMap = inputMap.erase(iterMap);
        }
        else{
            ++iterMap;
        }
        }
        
        //std::cout <<"inputMap.begin() = "; 
        //std::cout << "inputMap.begin() = " << inputMap.begin() << std::endl;
        //std::cout << "inputMap.end() = " << inputMap.end() << std::endl;

        if( 0 == inputMap.size() ){
        return -1;
        }
        else{
            std::pair<std::string, int> min = *min_element(inputMap.begin(), inputMap.end(), CompareSecond_Min());
        
            return min.second;
        }
    }

    int getMaxForChannelAND(std::map<std::string, int> inputMap){
        //Variable Declaration
        int iRetVal;
       
        std::pair<std::string, int> min = *min_element(inputMap.begin(), inputMap.end(), CompareSecond_Min());

        //Require All Elements to be nonzero (i.e. have a signal)
        if ( min.second > 0 ) {
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
