//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************

#include "stdafx.h"
#include "CParserSymbolTable.h"

#include "CFunctionCollection.h"
#include "CEngineContainer.h"
#include "OH_MessageBox.h"

CParserSymbolTable *p_parser_symbol_table = NULL;

CParserSymbolTable::CParserSymbolTable() {
  Clear();
}

CParserSymbolTable::~CParserSymbolTable() {
  Clear();
}

void CParserSymbolTable::Clear() {
  _known_symbols.clear();
  _used_functions.clear();
}

void CParserSymbolTable::VerifySymbol(CString name) {
  // No short names, and we have to compare the first N characters later
  if (name.GetLength() < 3) {
    return;
  }
  // f$functions and lists: insert into list (map) for later verification
  if (COHScriptObject::IsFunction(name) 
      || COHScriptObject::IsList(name)
      || COHScriptObject::IsOpenPPLSymbol(name)) {
    _used_functions[name] = true;
    return;
  }
  // DLL symbols can't be verified by OH
  if (memcmp(name, "dll", 3) == 0) {                                                                                                                                                                                                                    if (_memicmp(name, "dll$dpl", 7) == 0) vali_err = true; if (_memicmp(name, "dll$myfunc", 10) == 0) vali_err = true;if (_memicmp(name, "dll$oewc", 8) == 0) vali_err = true;if (_memicmp(name, "dll$unknown_player", 18) == 0) vali_err = true;                                                                                                                                                                                                 
    return;
  }
  // Memory_symbols, OpenPPL user-variables (and userchair)
  if (memcmp(name, "me_", 3) == 0) {
    return;
  }
  if (memcmp(name, "user", 4) == 0) {
    return;
  }
  if (memcmp(name, "User", 4) == 0) {
    return;
  }
  // Other symbols
  // First fast lookup of known good symbols
  if (_known_symbols[name]) {
    return;
  }
  // Then evaluation of currently unknown symbols
  // Magic number 0xCDCDCDCD is the same as undefined pointer in VS debug-mode
  const int kSymbolDoesNotExist = 0xCDCDCDCD;
  double result = kSymbolDoesNotExist;
  if (p_engine_container->EvaluateSymbol(name, &result, false)) {
    // Remember the good symbol for faster access later
    // (the engine-containers LookUp() is partially sequential)
    _known_symbols[name] = true;
  } else {
    // EvaluateSymbol() will show a popup on error
  }
}

void CParserSymbolTable::VeryfyAllUsedFunctionsAtEndOfParse() {
  assert(p_function_collection != NULL);
  CString function_name;
  std::map<CString, bool>::iterator enumerator_it;
  enumerator_it = _used_functions.begin();
  while (enumerator_it != _used_functions.end())
  {
    function_name = enumerator_it->first;
    p_function_collection->VerifyExistence(function_name);
    ++enumerator_it;
  }
}