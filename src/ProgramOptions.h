#include <string>
#include <vector>
#include <map>

class ProgramOptions
{
public:
    typedef std::vector<std::string>::iterator ArgsIterator ;
    typedef std::map<const char, std::string>::iterator AliasesIterator;

    ProgramOptions( int argc, char** argv );

    void AddAlias( const char shortName, const char* longName );

    const std::string GetArg( const char* arg );

    const int GetIntArg( const char* arg );

    const std::vector<std::string> GetMultiArg( const char* arg );

    const bool HasArg( const char* arg );

    ArgsIterator GetArgIterator( const char* arg );

private:
    std::vector<std::string> args;
    std::map<const char, std::string> aliases;
};
