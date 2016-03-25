#ifndef _GYCALLBACK_H_
#define _GYCALLBACK_H_

class gyCallback;
class gyCallbackData
{
public:
  gyCallbackData(gyCallback* sender=0):callbackSender(sender){}
  gyCallback* callbackSender;
};

class gyCallbackHandler : public gyRefCounted
{
public:
  virtual ~gyCallbackHandler(){}
  virtual void OnCallback(gyCallbackData* cbData) = 0;
};

class gyCallback
{
public:
  ~gyCallback();
  void Release();
  void Subscribe( gyCallbackHandler* handler );
  void Unsubscribe( gyCallbackHandler* handler );
  void Trigger( gyCallbackData* data );

private:
  typedef std::vector< gySharedPtr<gyCallbackHandler> > SuscriptorList;
  SuscriptorList suscriptors;
};


#endif