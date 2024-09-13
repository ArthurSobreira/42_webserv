#ifndef HTTPERROR_HPP
# define HTTPERROR_HPP

# include <string>
# include <map>

class HttpError
{
public:
    HttpError(int status_code, const std::string &custom_message = "");
    HttpError(HttpError const &src);
    HttpError &operator=(HttpError const &rhs);
    ~HttpError();
    std::string getBody() const;

private:
    int _status_code;
    std::string _custom_message;
    std::map<int, std::string> _status_messages;
    void _initStatusMessages();
};

#endif
