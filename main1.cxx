#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>

std::string LogFileName = "log1.txt";

void printDate(std::string date){
    std::string unit = "/ :: ";
    int i;

    for(i = 0; i < date.length() - 10; i++){
        std::cout << date[i];
    }
    std::cout << "/";

    for(int j = 0; j < 5; j++){
        for(int k = 0; k < 2; k++)
            std::cout << date[i + (j*2) + k];
        std::cout << unit[j];
    }
}

int main(void){
    std::string str, logdata[3];
    //IPのリスト
    std::vector<std::string> IPs(0);
    int i = 0;

    //IPリストの作成
    std::ifstream ifs(LogFileName);
    if(!ifs){
        std::cerr << "File \"" << LogFileName << "\" can't open." << std::endl;
        exit(1);
    }

    while(getline(ifs, str)){
        std::string strtmp;
        std::istringstream stream(str);
        int j = 0;
        bool newFlag = true;

        while(getline(stream, strtmp, ',')){
            logdata[j] = strtmp;
            j++;
        }

        //IPリストに無ければ追加
        for(j = 0; j < IPs.size(); j++){
            if(IPs[j] == logdata[1])
                newFlag = false;
        }
        if(newFlag){
            IPs.push_back(logdata[1]);
            i++;
        }
    }

    //故障期間を調べる
    for(i = 0; i < IPs.size(); i++){
        bool timeoutFlag = false;
        bool noAccident = true;
        //ファイル先頭に戻る
        ifs.clear();
        ifs.seekg(0, std::ios_base::beg);
        while(getline(ifs, str)){
            std::string strtmp;
            std::istringstream stream(str);
            int j = 0;

            while(getline(stream, strtmp, ',')){
                logdata[j] = strtmp;
                j++;
            }

            if(IPs[i] == logdata[1]){
                //故障が始まったか判定
                if(logdata[2] == "-" && !timeoutFlag){
                    timeoutFlag = true;
                    if(noAccident){
                        noAccident = false;
                        std::cout << "\n\nIP: " << logdata[1]
                                  << "\n故障期間:";
                    }
                    std::cout << "\n";
                    printDate(logdata[0]);
                    std::cout << "～ ";
                }

                //故障が終わったか判定
                if(timeoutFlag && !(logdata[2] == "-")){
                    printDate(logdata[0]);
                    timeoutFlag = false;
                }
            }
        }
    }
}
