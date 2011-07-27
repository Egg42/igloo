
//          Copyright Joakim Karlsson & Kim Gräsman 2010-2011.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef IGLOO_CONTEXTREGISTRY_H
#define IGLOO_CONTEXTREGISTRY_H

namespace igloo {

  template <typename ContextToCall>
  struct ContextRegistry  
  {
    static void RegisterSpec(const std::string& name, void (ContextToCall::*spec)())
    {
      GetSpecs().insert(std::make_pair(name, spec));
    }

    template <typename ContextToCreate>
    static void Run(const std::string& contextName, TestResults& results)
    {    
      const Specs& specs = GetSpecs();
      CallSpecs<ContextToCreate>(specs, contextName, results);
    }

    typedef void (ContextToCall::*SpecPtr)();
    typedef std::map<std::string, SpecPtr> Specs;
    
    template <typename ContextToCreate>
    static void CallSpecs(const Specs& specs, const std::string& contextName, TestResults& results)
    {
      ContextToCreate c;

      typename Specs::const_iterator it;
      for (it = specs.begin(); it != specs.end(); it++)
      {
        ContextToCreate context;
        context.SetName(contextName);
        
        if(CallSpec(context, (*it).first, (*it).second, results))
        {
          std::cout << ".";
        }
        else
        {
          std::cout << "F";
        }
      }
    }

    static bool CallSpec(ContextToCall& context, const std::string& specName, SpecPtr spec, TestResults& results)
    {
      bool result = true;
      
      try
      {
        context.IglooFrameworkSetUp();
        (context.*spec)();
       }
      catch (const AssertionException& e)
      {
        results.AddResult(TestResultFactory(context.Name(), specName).CreateFromException(e));
        result = false;
      }
      
      try 
      {
        context.IglooFrameworkTearDown();
      }
      catch (const AssertionException& e) 
      {
        results.AddResult(TestResultFactory(context.Name(), specName).CreateFromException(e));
        result = false;
      }
      
      if(result)
      {
        results.AddResult(TestResultFactory(context.Name(), specName).CreateSuccessful());
      }
      
      return result;
    }

    static Specs& GetSpecs()
    {
      static Specs specs;
      return specs;
    }
  };  
}
#endif
