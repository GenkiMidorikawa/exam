#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>

std::string LogFileName = "log3.txt";

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
    int timeoutLimit, pingLimit, slowNum; //N, t, m

    std::cout << "故障と判断するタイムアウト数N: ";
    std::cin >> timeoutLimit;
    std::cout << "過負荷と見なさない平均応答時間の上限t: ";
    std::cin >> pingLimit;
    std::cout << "過負荷かどうかの判定に用いる直近の応答回数m: ";
    std::cin >> slowNum;

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

    //故障・過負荷状態期間を調べる
    for(i = 0; i < IPs.size(); i++){
        int timeoutNum = 0;
        std::string tmp_date_timeout, tmp_date_slow;
        bool accidentFlag = false;
        bool noTimeout = true;
        bool overloadFlag = false;
        bool noOverload = true;
        bool IPprinted = false;
        std::vector<int> ping(0);
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
                tmp_date_slow = logdata[0];
                //タイムアウトの場合はpingを消去
                if(logdata[2] == "-"){
                    ping.clear();
                } else {
                    //直近m回のpingを保持
                    ping.push_back(std::stoi(logdata[2]));
                }
                //mより要素数が多ければ先頭の要素消去
                if(ping.size() > slowNum)
                    ping.erase(std::cbegin(ping));
                //pingの平均を計算
                if(ping.size() == slowNum){
                    double ave = 0;
                    for(int k = 0; k < slowNum; k++){
                        ave += (double)(ping[k]);
                    }
                    ave /= slowNum;

                    //過負荷状態が始まったか判定
                    if(!overloadFlag && ave > pingLimit){
                        overloadFlag = true;
                        if(noOverload){
                            noOverload = false;
                            noTimeout = true;
                            if(!IPprinted){
                                std::cout << "\n\nIP: " << logdata[1];
                                IPprinted = true;
                            }
                            std::cout <<"\n過負荷状態期間:";
                        }
                        std::cout << "\n";
                        printDate(tmp_date_slow);
                        std::cout << "～ ";
                    }

                    //過負荷状態が終わったか判定
                    if(overloadFlag && ave <= pingLimit){
                        printDate(logdata[0]);
                        overloadFlag = false;
                    }
                }

                //過負荷状態からそのまま故障するケース
                if(overloadFlag && logdata[2] == "-"){
                    printDate(tmp_date_slow);
                    overloadFlag = false;
                }

                //故障が始まったか判定
                if(logdata[2] == "-" && !accidentFlag){
                    ping.clear();
                    timeoutNum++;
                    if(timeoutNum == 1)
                        tmp_date_timeout = logdata[0];
                    if(timeoutNum == timeoutLimit){
                        accidentFlag = true;
                        if(noTimeout){
                            noTimeout = false;
                            noOverload = true;
                            if(!IPprinted){
                                std::cout << "\n\nIP: " << logdata[1];
                                IPprinted = true;
                            }
                            std::cout <<"\n故障期間:";
                        }
                        std::cout << "\n";
                        printDate(tmp_date_timeout);
                        std::cout << "～ ";
                        timeoutNum = 0;
                    }
                }

                //故障が終わったか判定
                if(accidentFlag && !(logdata[2] == "-")){
                    printDate(logdata[0]);
                    accidentFlag = false;
                }
            }
        }
        timeoutNum = 0;
    }
}
