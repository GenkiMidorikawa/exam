#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

std::string LogFileName = "log1.txt";

void printDate(std::string date){
    std::string unit = "月日時分秒";
    int i;

    for(i = 0; i < date.length() - 10; i++){
        std::cout << date[i];
    }
    std::cout << "年";

    for(int j = 0; j < 5; j++){
        for(int k = 0; k < 2; k++)
            std::cout << date[i+j+k];
        std::cout << unit[j];
    }
}

int main(void){
    std::string str, logdata[3];
    //IPのリスト
    std::string* IPs;
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

        while(getline(stream, strtmp, ',')){
            logdata[j] = strtmp;
            j++;
        }

        IPs[i] = logdata[1];
        i++;
    }
    i = 0;

    //故障期間を調べる
    for(i = 0; i < sizeof(IPs) / sizeof(std::string); i++){
        //ファイル先頭に戻る
        ifs.seekg(0, std::ios_base::beg);
        while(getline(ifs, str)){
            std::string strtmp;
            std::istringstream stream(str);
            int j = 0;
            bool accidentFlag = false;
            bool noAccident = true;

            while(getline(stream, strtmp, ',')){
                logdata[j] = strtmp;
                j++;
            }

            if(IPs[i] == logdata[1]){
                //故障が始まったか判定
                if(logdata[2] == "-" && !accidentFlag){
                    accidentFlag = true;
                    if(noAccident){
                        noAccident = false;
                        std::cout << "IP: " << logdata[1]
                                  << "\n故障期間:";
                    }
                    std::cout << "\n";
                    printDate(logdata[0]);
                    std::cout << " ～ ";
                }

                //故障が終わったか判定
                if(accidentFlag && logdata[2] != "-"){
                    printDate(logdata[0]);
                }
            }
            i++;
        }
        std::cout << "\n";
    }
}
