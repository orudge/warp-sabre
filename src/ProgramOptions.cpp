#include "ProgramOptions.h"

#include <iostream>

ProgramOptions::ProgramOptions( int argc, char** argv )
{
    for( int i = 1; i < argc; i++ ) {
        args.push_back( argv[ i ] );
        // std::cout << "loaded arg " << argv[i] << std::endl;
    }
}

void ProgramOptions::AddAlias( const char shortName, const char* longName )
{
    aliases[ shortName ] = longName;
}

const std::string ProgramOptions::GetArg( const char* arg )
{
    std::string result;
    ArgsIterator it = GetArgIterator( arg );
    if( it != args.end() ) {
        result = *it;
    }
    return result;
}

const int ProgramOptions::GetIntArg( const char* arg )
{
    int result = 0;
    ArgsIterator it = GetArgIterator( arg );
    if( it != args.end() ) {
        result = stoi( *it );
    }
    return result;
}

const std::vector<std::string> ProgramOptions::GetMultiArg( const char* arg )
{
    std::vector<std::string> result;
    ArgsIterator it = GetArgIterator( arg );
    if( it != args.end() ) {
        for( ; it != args.end(); it++ ) {
            std::string& localarg = *it;
            //std::cout << "GetMultiArg " << arg << " " << localarg << std::endl;
            if( localarg.begin() != localarg.end() && *localarg.begin() == '-' ) {
                return result;
            } else {
                result.push_back( localarg );
            }
        }
    }

    return result;
}

const bool ProgramOptions::HasArg( const char* arg )
{
    return GetArgIterator( arg ) != args.end();
}

// Return the position in the arg list after this one
ProgramOptions::ArgsIterator ProgramOptions::GetArgIterator( const char* arg )
{
    //std::cout << "GetArgIterator " << arg << std::endl;
    for( ArgsIterator it = args.begin(); it != args.end(); it++ ) {
        std::string& locarg = *it;
        std::string::iterator ch = locarg.begin();
        //std::cout << "GetArgIterator " << locarg << " " << *ch << std::endl;
        if( *ch == '-' ) {
            if( *++ch == '-' ) {
                // Double switch - check the full name
                std::string cmdSwitch( ++ch, locarg.end() );
                //std::cout << "GetArgIterator double switch " << cmdSwitch << std::endl;
                if( arg && cmdSwitch == arg ) {
                    return ++it;
                } else {
                    if( ++it == args.end() ) {
                        return args.end();
                    }
                }
            } else {
                // Single switch - check the short name
                const char cmdSwitch = *ch;
                //std::cout << "GetArgIterator single switch " << locarg << " " << cmdSwitch << std::endl;
                AliasesIterator ita = aliases.find( cmdSwitch );
                if( ita != aliases.end() ) {
                    //std::cout << "GetArgIterator alias " << ita->first << " " << ita->second << std::endl;
                    if( arg && ita->second == arg ) {
                        return ++it;
                    } else {
                        if( ++it == args.end() ) {
                            return args.end();
                        }
                    }
                }
            }
        } else {
            //std::cout << "empty arg" << std::endl;
            if( !arg && it != args.end() ) {
                return it;
            }
        }
    }
    return args.end();
}