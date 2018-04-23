#ifndef MESSAGE_H
#define MESSAGE_H
#include <QWidget>

class Message
{
public:
    Message();
    int amountOpenBytes, amountCloseBytes, amountdataBytes;
    int msglength;
    int startByte = 0xA;
    int closeByte = 0xB;
    char sending[255];
    void saltMessage (const int &amoutOpenBytes);
    void PrintMessage(char* sending);
    void UsersInput();

private:
    void pepperMessage (int &amoutOpenBytes, const int &endByte);
    void collectMessage ();
};
#endif // MESSAGE_H


