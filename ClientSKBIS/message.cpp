#include "message.h"

void Message::saltMessage(const int &amoutOpenBytes)
{
    for(int i=0; i<=amoutOpenBytes; i++)
    {
        int salt = rand()%255;
        if (salt!=0xA)
        {
            Message::sending[i]= salt;
        }
        else
        {
            --i;
        }
    };
}

void Message::pepperMessage(int &amoutOpenBytes, const int &endByte)
{
    for(int i=0; i<=amoutOpenBytes; i++)
        {
            int salt = rand()%255;
            if (salt!=0xB)
            {
                Message::sending[255]= salt;
            }
            else
            {
                --i;
            }
        };
}

void Message::collectMessage()
{

}
void Message::PrintMessage(char* sending)
{
    //for (int i=0; i<5; i++)
    //    {
    //        this->sending[i]<< " ";
    //   }
}
