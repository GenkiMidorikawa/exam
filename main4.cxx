#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include<cmath>

std::string LogFileName = "log4.txt";

//日付時刻を表示
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

//サブネットの開始IPを返す
std::string getHeadIP(std::string IP){
        int IPCursor, IPPrefixLength, IPRange, tmp_int;
        std::string headIP, tmp_str;

        //プレフィックス長を見つける
        IPCursor = IP.find("/");
        for(int i = 1; i <= IP.length() - IPCursor; i++){
            tmp_str += IP[IPCursor+i];
        }
        IPPrefixLength = std::stoi(tmp_str);
        tmp_str.clear();

        //開始IPを決定
        if(IPPrefixLength / 8 == 4){
            headIP = IP;
        } else {
            IPCursor = 0;
            //アドレスの数字が固定の部分を特定
            for(int i = 0; i < IPPrefixLength / 8; i++){
                IPCursor = IP.find(".", IPCursor);
                IPCursor++;
            }
            //固定の部分をコピー
            for(int i = 0; i < IPCursor; i++){
                headIP += IP[i];
            }
            //アドレスの数字が可変な部分の最初の1区切りをコピー
            tmp_int = IP.find_first_of("./", IPCursor);
            for(int i = 0; i < tmp_int - IPCursor; i++){
                tmp_str += IP[IPCursor+i];
            }
            tmp_int = std::stoi(tmp_str);
            tmp_str.clear();

            IPRange = std::pow(2.0, (double)(8 - (IPPrefixLength % 8)));
            //開始IP計算
            tmp_int = (int)(tmp_int / IPRange) * IPRange;
            tmp_str = std::to_string(tmp_int);
            headIP += tmp_str;
            //開始IPの残りの部分を追加
            for(int i = 0; i < 3 - (int)(IPPrefixLength / 8); i++){
                headIP += ".0";
            }
            headIP = headIP + "/" + std::to_string(IPPrefixLength);
        }

        return headIP;
}

int main(void){
    std::string str, logdata[3];
    //IPのリスト
    std::vector<std::string> IPs(0);
    //サブネットのリスト
    std::vector<std::string> Subnets(0);
    int timeoutLimit, pingLimit, slowNum; //N, t, m

    std::cout << "故障と判断するタイムアウト数N: ";
    std::cin >> timeoutLimit;
    std::cout << "過負荷と見なさない平均応答時間の上限t: ";
    std::cin >> pingLimit;
    std::cout << "過負荷かどうかの判定に用いる直近の応答回数m: ";
    std::cin >> slowNum;

    //IP・サブネットのリストの作成
    std::ifstream ifs(LogFileName);
    if(!ifs){
        std::cerr << "File \"" << LogFileName << "\" can't open." << std::endl;
        exit(1);
    }

    while(getline(ifs, str)){
        std::string strtmp, IPtmp;
        std::istringstream stream(str);
        int i = 0;
        bool newFlag = true;

        while(getline(stream, strtmp, ',')){
            logdata[i] = strtmp;
            i++;
        }

        IPtmp = getHeadIP(logdata[1]);

        //サブネットリストに無ければ追加
        for(i = 0; i < Subnets.size(); i++){
            if(Subnets[i] == IPtmp)
                newFlag = false;
        }
        if(newFlag){
            Subnets.push_back(IPtmp);
        }

        newFlag = true;

        //IPリストに無ければ追加
        for(i = 0; i < IPs.size(); i++){
            if(IPs[i] == logdata[1])
                newFlag = false;
        }
        if(newFlag){
            IPs.push_back(logdata[1]);
        }
    }

    //過負荷状態期間を調べる
    for(int i = 0; i < IPs.size(); i++){
        int timeoutNum = 0;
        std::string tmp_date_timeout, tmp_date_slow;
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
            }
        }
    }

    //故障期間を調べる
    for(int i = 0; i < Subnets.size(); i++){
        int timeoutNum = 0;
        std::string tmp_date_timeout, tmp_date_slow;
        bool timeoutFlag = false;
        bool noTimeout = true;
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
            if(Subnets[i] == getHeadIP(logdata[1])){
                if(logdata[2] != "-"){
                    //タイムアウトのカウントリセット
                    timeoutNum = 0;
                }

                //故障が始まったか判定
                if(logdata[2] == "-" && !timeoutFlag){
                    ping.clear();
                    timeoutNum++;
                    if(timeoutNum == 1)
                        tmp_date_timeout = logdata[0];
                    if(timeoutNum == timeoutLimit){
                        timeoutFlag = true;
                        if(noTimeout){
                            noTimeout = false;
                            if(!IPprinted){
                                std::cout << "\n\nIP: " << logdata[1];
                                IPprinted = true;
                            }
                            std::cout <<"\n故障期間(サブネット毎):";
                        }
                        std::cout << "\n";
                        printDate(tmp_date_timeout);
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
