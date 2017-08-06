#ifndef MULTIDLA_LOGGER_H
#define	MULTIDLA_LOGGER_H

enum LogLvl { LL_RELEASE, LL_DEBUG_1, LL_DEBUG_2, LL_DEBUG_3, LL_LAST };

class Logger
{
public:
    Logger();
    virtual ~Logger();

    void    error();
    void    info();
    void    debug();
    void    warning();
    void    critical();
    void    insane();
    
private:
    void    printToLog();
};

#endif	// MULTIDLA_LOGGER_H
