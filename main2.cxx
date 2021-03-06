#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>

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
    std::string LogFileName = "log2.txt";
    std::string str, logdata[3];
    //IPのリスト
    std::vector<std::string> IPs(0);
    int timeoutLimit; //N

    std::cout << "故障と判断するタイムアウト数N: ";
    std::cin >> timeoutLimit;

    //IPリストの作成
    std::ifstream ifs(LogFileName);
    if(!ifs){
        std::cerr << "File \"" << LogFileName << "\" can't open." << std::endl;
        exit(1);
    }

    while(getline(ifs, str)){
        std::istringstream stream(str);
        int i = 0;
        bool newFlag = true;

        while(getline(stream, logdata[i], ',')){
            i++;
        }

        //IPリストに無ければ追加
        for(i = 0; i < IPs.size(); i++){
            if(IPs[i] == logdata[1])
                newFlag = false;
        }
        if(newFlag){
            IPs.push_back(logdata[1]);
        }
    }

    //故障期間を調べる
    for(int i = 0; i < IPs.size(); i++){
        int timeoutNum = 0;
        std::string tmp_date;
        bool timeoutFlag = false;
        bool noTimeout = true;
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
                if(logdata[2] != "-"){
                    //タイムアウトのカウントリセット
                    timeoutNum = 0;
                }

                //故障が始まったか判定
                if(logdata[2] == "-" && !timeoutFlag){
                    timeoutNum++;
                    if(timeoutNum == 1){
                        tmp_date = logdata[0];
                    }
                    if(timeoutNum == timeoutLimit){
                        timeoutFlag = true;
                        if(noTimeout){
                            noTimeout = false;
                            std::cout << "\n\nIP: " << logdata[1]
                                    << "\n故障期間:";
                        }
                        std::cout << "\n";
                        printDate(tmp_date);
                        std::cout << "～ ";
                        timeoutNum = 0;
                    }
                }

                //故障が終わったか判定
                if(timeoutFlag && logdata[2] != "-"){
                    printDate(logdata[0]);
                    timeoutFlag = false;
                }
            }
        }
    }

    std::cout << std::endl;
    return 0;
}
