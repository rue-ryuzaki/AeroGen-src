#ifndef MULTIDLA_LOGGER_H
#define	MULTIDLA_LOGGER_H

enum LogLvl { LL_RELEASE, LL_DEBUG_1, LL_DEBUG_2, LL_DEBUG_3, LL_LAST };

class Logger
{
public:
    Logger();
    virtual ~Logger();

    void Error();
    void Info();
    void Debug();
    void Warning();
    void Critical();
    void Insane();
    
private:
    void PrintToLog();
};

#endif	/* MULTIDLA_LOGGER_H */
