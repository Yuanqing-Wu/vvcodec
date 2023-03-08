#pragma once

#include <sstream>
#include <string>
#include <list>
#include <map>

#include <algorithm>
#include <regex>

struct OptionBase {
    OptionBase(const std::string& name, const std::string& desc)
    : opt_string(name), opt_desc(desc)
    {};

    virtual ~OptionBase() {}

    /* parse argument arg, to obtain a value for the option */
    virtual void parse(const std::string& arg) = 0;
    /* set the argument to the default value */
    virtual void setDefault() = 0;
    virtual const std::string getDefault( ) { return std::string(); }
    virtual const std::string getValue( ) { return std::string(); }

    std::string opt_string;
    std::string opt_desc;
};

/** Type specific option storage */
template<typename T>
struct Option : public OptionBase
{
  Option(const std::string& name, T& storage, T default_val, const std::string& desc)
  : OptionBase(name, desc), opt_storage(storage), opt_default_val(default_val)
  {}

  void parse(const std::string& arg);

  void setDefault()
  {
    opt_storage = opt_default_val;
  }
  virtual const std::string getDefault( );
  virtual const std::string getValue  ( );

  T& opt_storage;
  T opt_default_val;
};

template<typename T>
inline
const std::string Option<T>::getValue( )
{
  std::ostringstream oss;
  oss << opt_storage;
  return oss.str();
}

template<>
inline
const std::string Option<std::string>::getValue( )
{
  std::ostringstream oss;
  if( opt_storage.empty() )
  {
    oss << "\"\"";
  }
  else
  {
    oss << opt_storage;
  }
  return oss.str();
}

template<typename T>
inline 
const std::string Option<T>::getDefault( )
{ 
  std::ostringstream oss;
  oss << opt_default_val;
  return oss.str(); 
}

/* Generic parsing */
template<typename T>
inline void
Option<T>::parse(const std::string& arg, ErrorReporter&) {
    std::string param = arg;

    if (arg == "")
    {
        param = "''";
    }

    std::istringstream arg_ss(param, std::istringstream::in);
    arg_ss.exceptions(std::ios::failbit);
    try
    {
        arg_ss >> opt_storage;
    }
    catch (...)
    {
        throw ParseFailure(opt_string, param);
    }
}

template<>
inline void
Option<std::string>::parse(const std::string& arg, ErrorReporter&) {
    opt_storage = arg;
}

template<>
inline void
Option<bool>::parse(const std::string& arg, ErrorReporter&) {
    if (arg.empty())
    {
        opt_storage = true;
    }
    else
    {
        std::istringstream arg_ss(arg, std::istringstream::in);
        arg_ss.exceptions(std::ios::failbit);
        try
        {
            arg_ss >> opt_storage;
        }
        catch (...)
        {
            throw ParseFailure(opt_string, arg);
        }
    }
}

struct Options {
    ~Options() {
        for(Options::NamesPtrList::iterator it = opt_list.begin(); it != opt_list.end(); it++) {
            delete *it;
        }
    }
    struct Names {
        Names() : opt(0) {};
        ~Names() {
            if (opt) {
                delete opt;
            }
        }
        std::list<std::string> opt_long;
        std::list<std::string> opt_short;
        OptionBase* opt;
    };

    void addOption(OptionBase *opt) {
        Names* names = new Names();
        names->opt = opt;
        std::string& opt_string = opt->opt_string;

        size_t opt_start = 0;
        for (size_t opt_end = 0; opt_end != std::string::npos;) {
            opt_end = opt_string.find_first_of(',', opt_start);
            bool force_short = 0;
            if (opt_string[opt_start] == '-') {
                opt_start++;
                force_short = 1;
            }
            std::string opt_name = opt_string.substr(opt_start, opt_end - opt_start);
            if (force_short || opt_name.size() == 1) {
                names->opt_short.push_back(opt_name);
                opt_short_map[opt_name].push_back(names);
            }
            else {
                names->opt_long.push_back(opt_name);

                std::string optLongLower = opt_name;
                std::transform( optLongLower.begin(), optLongLower.end(), optLongLower.begin(), ::tolower );
                opt_long_map[optLongLower].push_back(names);
            }
            opt_start += opt_end + 1;
        }
        opt_list.push_back(names);
    }

    typedef std::list<Names*> NamesPtrList;
    NamesPtrList opt_list;

    typedef std::map<std::string, NamesPtrList> NamesMap;
    NamesMap opt_long_map;
    NamesMap opt_short_map;

    template<typename T>
    Options operator()(const std::string& name, T& storage, const std::string& desc = "") {
        addOption(new Option<T>(name, storage, storage, desc));
        return Options(*this);
    }
};

struct OptionFunc : public OptionBase {
    typedef void (Func)(Options&);

    OptionFunc(const std::string& name, Options& parent_, Func *func_, const std::string& desc)
    : OptionBase(name, desc), parent(parent_), func(func_)
    {}

    void parse(const std::string& arg) {
        func(parent, arg);
    }

    void setDefault() {
        return;
    }

private:
    Options& parent;
    Func* func;
};

static void setOptions(Options::NamesPtrList& opt_list, const std::string& value)
{
  /* multiple options may be registered for the same name:
    *   allow each to parse value */
  for (Options::NamesPtrList::iterator it = opt_list.begin(); it != opt_list.end(); ++it)
  {
    (*it)->opt->parse(value);
  }
}

struct OptionWriter
{
  OptionWriter(Options& rOpts)
  : opts(rOpts)
  {}
  virtual ~OptionWriter() {}

  virtual const std::string where() = 0;

  bool storePair(bool allow_long, bool allow_short, const std::string& name, const std::string& value)
  {
    bool found = false;
    Options::NamesMap::iterator opt_it;
    if (allow_long)
    {
      std::string optLongLower = name;
      std::transform( optLongLower.begin(), optLongLower.end(), optLongLower.begin(), ::tolower );

      opt_it = opts.opt_long_map.find(optLongLower);
      if (opt_it != opts.opt_long_map.end())
      {
        found = true;
      }
    }

    /* check for the short list */
    if (allow_short && !(found && allow_long))
    {
      opt_it = opts.opt_short_map.find(name);
      if (opt_it != opts.opt_short_map.end())
      {
        found = true;
      }
    }

    if (!found)
    {
      return false;
    }

    setOptions((*opt_it).second, value);
    return true;
  }

  bool storePair(const std::string& name, const std::string& value)
  {
    return storePair(true, true, name, value);
  }

  Options& opts;
};

struct ArgvParser : public OptionWriter
{
  ArgvParser(Options& rOpts)
  : OptionWriter(rOpts)
  {}

  const std::string where() { return "command line"; }

  /**
   * returns number of extra arguments consumed
   */
  unsigned parseGNU(unsigned argc, const char* argv[])
  {
    /* gnu style long options can take the forms:
    *  --option=arg
    *  --option arg
    */
    std::string arg(argv[0]);
    size_t arg_opt_start = arg.find_first_not_of('-');
    size_t arg_opt_sep = arg.find_first_of('=');
    std::string option = arg.substr(arg_opt_start, arg_opt_sep - arg_opt_start);

    std::transform( option.begin(), option.end(), option.begin(), ::tolower ); // compare option always in lower case

    unsigned extra_argc_consumed = 0;
    if (arg_opt_sep == std::string::npos)
    {
      // check if we have an argument 
      if( argc > 1)
      {
        std::string val(argv[1]);
        size_t val_sep = val.find_first_of('-');
        //check if either have no - or the parameter is a number 
        if( 0 != val_sep || std::regex_match( val, std::regex( ( "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?" ) ) ) ) 
        {
          extra_argc_consumed++;
          /* argument occurs after option_sep */
          storePair(true, false, option, val);
          return extra_argc_consumed;
        }
        else if( val_sep == 0 && val.size() == 1 )
        {
          extra_argc_consumed++;
          /* argument occurs after option_sep */
          storePair(true, false, option, val);
          return extra_argc_consumed;
        }
      }

      /* no argument found => argument in argv[1] (maybe) */
      /* xxx, need to handle case where option isn't required */
      if(!storePair(true, false, option, ""))
      {
        return 0;
      }
    }
    else
    {
      /* argument occurs after option_sep */
      std::string val = arg.substr(arg_opt_sep + 1);
      storePair(true, false, option, val);
    }

    return extra_argc_consumed;
  }

    unsigned parseSHORT(unsigned argc, const char* argv[])
  {
    /* short options can take the forms:
    *  --option arg
    *  -option arg
    */
    std::string arg(argv[0]);
    size_t arg_opt_start = arg.find_first_not_of('-');
    std::string option = arg.substr(arg_opt_start);
    /* lookup option */

    /* argument in argv[1] */
    /* xxx, need to handle case where option isn't required */
    if (argc == 1)
    {
      storePair(false, true, option, std::string(""));
      return 1;
    }

    std::string argNext = argv[1];
    if( !argNext.empty() && argNext[0] == '-' )
    {
      // check if bool switch and check if next param is not an option
      if( argNext.size() > 1 )
      {
        if( argNext[1] == '-' ) // is long option --
        {
          storePair(false, true, option, std::string(""));
          return 0;
        }

        // check if argv is an digit number
        if( !std::isdigit(argNext[1]) )
        {
          storePair(false, true, option, std::string(""));
          return 0;
        }
      }
    }

    storePair(false, true, option, std::string(argv[1]));

    return 1;
  }
};

inline std::list<const char*> scanArgv(Options& opts, unsigned argc, const char* argv[])
{
  ArgvParser avp(opts);

  /* a list for anything that didn't get handled as an option */
  std::list<const char*> non_option_arguments;

  for(unsigned i = 0; i < argc; i++)
  {
    if (argv[i][0] != '-')
    {
      non_option_arguments.push_back(argv[i]);
      continue;
    }

    if (argv[i][1] == 0)
    {
      /* a lone single dash is an argument (usually signifying stdin) */
      non_option_arguments.push_back(argv[i]);
      continue;
    }

    if (argv[i][1] != '-')
    {
      /* handle short (single dash) options */
      i += avp.parseSHORT(argc - i, &argv[i]);
      continue;
    }

    if (argv[i][2] == 0)
    {
      /* a lone double dash ends option processing */
      while (++i < argc)
      {
        non_option_arguments.push_back(argv[i]);
      }
      break;
    }

    /* handle long (double dash) options */
    i += avp.parseGNU(argc - i, &argv[i]);
  }

  return non_option_arguments;
}