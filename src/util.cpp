#include "util.h"

map<string, string> queryExtract()
{
    char* ptr = secure_getenv("QUERY_STRING");
    if (!ptr) {
        slogf(ERROR, "Abort! No Query String");
    }

    string query_str(ptr);
    auto query_arr = split(query_str, "&");

    map<string, string> querys;

    for (auto& str : query_arr) {
        auto tmp = split(str, "=");
        if (tmp.size() == 2) {
            querys[tmp[0]] = tmp[1];
        }
    }

    return querys;
}


vector<string> split(const string& source, const string& delim)
{
    vector<string> ans;
    size_t begin_pos = 0, end_pos = source.find(delim);
    while (end_pos != string::npos) {
        ans.push_back(source.substr(begin_pos, end_pos - begin_pos));
        begin_pos = end_pos + delim.size();
        end_pos = source.find(delim, begin_pos);
    }
    ans.push_back(source.substr(begin_pos, end_pos - begin_pos));
    return ans;
}

bool hasEnding(std::string const& fullString, std::string const& ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

int getConnSocket(string host, string port)
{
    if (host == "" || port == "")
        return -1;
    printf("Connect to %s/%s\n",host.c_str(), port.c_str());
    fflush(stdout);
    int p = stol(port);

    int c_fd, flags, ret;
    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));

    struct hostent *hptr;  
    if ((hptr = gethostbyname(host.c_str())) == NULL)   
    {   
        return -1;
    }  

    memcpy(&s_addr.sin_addr, hptr->h_addr_list[0], hptr->h_length);
    s_addr.sin_family = hptr->h_addrtype;
    s_addr.sin_port = htons(p);


    if ((c_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("create socket fail.\n");
        exit(0);
    }
    flags = fcntl(c_fd, F_GETFL, 0);
    if (flags < 0) {
        perror("get socket flags fail.\n");
        return -1;
    }

    if (fcntl(c_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("set socket O_NONBLOCK fail.\n");
        return -1;
    }
    ret = connect(c_fd, (struct sockaddr*)&s_addr, sizeof(struct sockaddr));
    if (ret < 0) {
        if (errno == EINPROGRESS) {
            return c_fd;
        } else {
            perror("connect remote server fail.\n");
            printf("%d\n", errno);
            exit(0);
        }
    }


    return c_fd;
}


