
#ifndef TEXTEXCEPTION_H
#define TEXTEXCEPTION_H

#include <iostream>

class text_exception final : public std::exception {
public:
    text_exception(const std::string& message): message{message}
    {}
    const char* what() const noexcept override
    {
        return message.c_str();    
    }
    
private:
    std::string message;
};

#endif //TEXTEXCEPTION_H
