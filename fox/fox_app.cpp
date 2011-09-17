#include "fox_app.h"

const FX::FXMetaClass fox_app::metaClass("fox_app", fox_app::manufacture, &FXApp::metaClass, NULL, 0, 0);

long fox_app::handle(FX::FXObject* sender,FX::FXSelector sel,void* ptr)
{ 
  return FXApp::handle(sender, sel, ptr);
  //  const FXMapEntry* me=(const FXMapEntry*)metaClass.search(sel);
  //  return me ? (this->* me->func)(sender,sel,ptr) : FXMainWindow::handle(sender,sel,ptr); 
};
